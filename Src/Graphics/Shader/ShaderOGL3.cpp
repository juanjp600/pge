#include "../GraphicsOGL3.h"

using namespace PGE;

ShaderOGL3::ShaderOGL3(Graphics* gfx, const FilePath& path) : resourceManager(gfx) {
    graphics = (GraphicsOGL3*)gfx;

    graphics->takeGlContext();

    filepath = path;

    std::vector<byte> vertexFile = (path + "vertex.glsl").readBytes();
    PGE_ASSERT(!vertexFile.empty(), "Failed to find vertex.glsl (filepath: " + path.str() + ")");
    vertexFile.push_back(0);
    String vertexSource((char*)vertexFile.data());
    glVertexShader = resourceManager.addNewResource<GLShader>(GL_VERTEX_SHADER, vertexSource);

    std::vector<byte> fragmentFile = (path + "fragment.glsl").readBytes();
    PGE_ASSERT(!fragmentFile.empty(), "Failed to find fragment shader (filepath: " + path.str() + ")");
    fragmentFile.push_back(0);
    String fragmentSource((char*)fragmentFile.data());
    glFragmentShader = resourceManager.addNewResource<GLShader>(GL_FRAGMENT_SHADER, fragmentSource);

    glShaderProgram = resourceManager.addNewResource<GLProgram>(std::vector{ glVertexShader.get(), glFragmentShader.get() });

    extractVertexUniforms(vertexSource);

    extractVertexAttributes(vertexSource);

    extractFragmentUniforms(fragmentSource);

    extractFragmentOutputs(fragmentSource);
}

void ShaderOGL3::extractVertexUniforms(const String& vertexSource) {
    std::vector<ParsedShaderVar> vertexUniforms;
    extractShaderVars(vertexSource, "uniform", vertexUniforms);
    for (int i = 0; i < (int)vertexUniforms.size(); i++) {
        vertexShaderConstants.emplace(
            vertexUniforms[i].name,
            ConstantOGL3(
                graphics,
                glGetUniformLocation(glShaderProgram, vertexUniforms[i].name.cstr()),
                parsedTypeToGlType(vertexUniforms[i].type),
                1 /* TODO: add array support */,
                vertexUniformData,
                String::Key(vertexUniforms[i].name)));
    }
}

void ShaderOGL3::extractVertexAttributes(const String& vertexSource) {
    std::vector<ParsedShaderVar> parsedAttribs;
    extractShaderVars(vertexSource, "in", parsedAttribs);

    std::vector<StructuredData::ElemLayout::Entry> layoutEntries;
    for (int i = 0; i < (int)parsedAttribs.size(); i++) {
        GLenum attrType = parsedTypeToGlType(parsedAttribs[i].type);
        GLenum attrElemType; int attrElemCount;
        decomposeGlType(attrType, attrElemType, attrElemCount);

        layoutEntries.emplace_back(parsedAttribs[i].name, glSizeToByteSize(attrType, attrElemCount));
        glVertexAttribLocations.emplace(
            String::Key(parsedAttribs[i].name),
            GlAttribLocation(
                glGetAttribLocation(glShaderProgram, parsedAttribs[i].name.cstr()),
                attrElemType,
                attrElemCount));
    }

    vertexLayout = StructuredData::ElemLayout(layoutEntries);
}

void ShaderOGL3::extractFragmentUniforms(const String& fragmentSource) {
    std::vector<ParsedShaderVar> fragmentUniforms;
    extractShaderVars(fragmentSource, "uniform", fragmentUniforms);
    for (int i = 0; i < (int)fragmentUniforms.size(); i++) {
        ConstantOGL3 constant = ConstantOGL3(
            graphics,
            glGetUniformLocation(glShaderProgram, fragmentUniforms[i].name.cstr()),
            parsedTypeToGlType(fragmentUniforms[i].type),
            1 /* TODO: add array support */,
            vertexUniformData,
            String::Key(fragmentUniforms[i].name));
        if (fragmentUniforms[i].type.equals("sampler2D")) {
            constant.setValue((u32)samplerConstants.size());
            samplerConstants.emplace(fragmentUniforms[i].name, constant);
        }
        else {
            fragmentShaderConstants.emplace(fragmentUniforms[i].name, constant);
        }
    }
}

void ShaderOGL3::extractFragmentOutputs(const String fragmentSource) {
    std::vector<ParsedShaderVar> fragmentOutputs;
    extractShaderVars(fragmentSource, "out", fragmentOutputs);
    for (int i = 0; i < (int)fragmentOutputs.size(); i++) {
        glBindFragDataLocation(glShaderProgram, i, fragmentOutputs[i].name.cstr());
    }
}

void ShaderOGL3::extractShaderVars(const String& src, const String& varKind, std::vector<ParsedShaderVar>& varList) {
    String line = "";
    String varStr = varKind + " ";
    for (char16 ch : src) {
        if (ch != '\r' && ch != '\n') {
            line += ch;
        }
        else {
            int minLen = varStr.length() < line.length() ? varStr.length() : line.length();
            if (line.substr(0, minLen).equals(varStr)) {
                bool typeHasBeenRead = false;
                ParsedShaderVar var;
                var.type = "";
                var.name = "";
                auto it = line.begin() + varStr.length();
                while (it != line.end()) {
                    char16 lineCh = *it;
                    if (lineCh == ' ') {
                        if (typeHasBeenRead && var.name.length() > 0) {
                            break;
                        }
                        typeHasBeenRead = true;
                    }
                    else {
                        if (lineCh == ';' || lineCh == '\r' || lineCh == '\n') {
                            break;
                        }
                        else {
                            if (typeHasBeenRead) {
                                var.name += lineCh;
                            }
                            else {
                                var.type += lineCh;
                            }
                        }
                    }
                    it++;
                }
                varList.push_back(var);
            }
            line = "";
        }
    }
}

const StructuredData::ElemLayout& PGE::ShaderOGL3::getVertexLayout() const {
    return vertexLayout;
}

int ShaderOGL3::glSizeToByteSize(GLenum type, int size) const {
    switch (type) {
        case GL_INT:
            return sizeof(GLint);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_FLOAT_VEC2:
            return sizeof(GLfloat) * 2;
        case GL_FLOAT_VEC3:
            return sizeof(GLfloat) * 3;
        case GL_FLOAT_VEC4:
            return sizeof(GLfloat) * 4;
        case GL_FLOAT_MAT4:
            return sizeof(GLfloat) * 4 * 4;
        default:
            throw PGE_CREATE_EX("Unsupported OpenGL datatype: " + String::fromInt(type));
    }
}

GLenum ShaderOGL3::parsedTypeToGlType(const String& parsedType) {
    if (parsedType.equals("float")) {
        return GL_FLOAT;
    } else if (parsedType.equals("vec2")) {
        return GL_FLOAT_VEC2;
    } else if (parsedType.equals("vec3")) {
        return GL_FLOAT_VEC3;
    } else if (parsedType.equals("vec4")) {
        return GL_FLOAT_VEC4;
    } else if (parsedType.equals("mat4")) {
        return GL_FLOAT_MAT4;
    } else if (parsedType.equals("int")) {
        return GL_INT;
    } else if (parsedType.equals("uint")) {
        return GL_UNSIGNED_INT;
    } else {
        throw PGE_CREATE_EX("Unsupported GLSL datatype: "+parsedType);
    }
}

void ShaderOGL3::decomposeGlType(GLenum compositeType, GLenum& elemType, int& elemCount) {
    switch (compositeType) {
        case GL_INT:
            elemType = GL_INT; elemCount = 1;
            break;
        case GL_UNSIGNED_INT:
            elemType = GL_UNSIGNED_INT; elemCount = 1;
            break;
        case GL_FLOAT:
            elemType = GL_FLOAT; elemCount = 1;
            break;
        case GL_FLOAT_VEC2:
            elemType = GL_FLOAT; elemCount = 2;
            break;
        case GL_FLOAT_VEC3:
            elemType = GL_FLOAT; elemCount = 3;
            break;
        case GL_FLOAT_VEC4:
            elemType = GL_FLOAT; elemCount = 4;
            break;
        case GL_FLOAT_MAT4:
            elemType = GL_FLOAT; elemCount = 4 * 4;
            break;
        default:
            throw PGE_CREATE_EX("Unsupported OpenGL datatype: " + String::fromInt(compositeType));
    }
}

void ShaderOGL3::useShader() {
    GLuint glError = GL_NO_ERROR;

    graphics->takeGlContext();

    glUseProgram(glShaderProgram);

    byte* ptr = nullptr;
    for (const auto& kvp : glVertexAttribLocations) {
        const String::Key& key = kvp.first;
        const GlAttribLocation& glAttribLocation = kvp.second;
        const StructuredData::ElemLayout::LocationAndSize& locationAndSizeInBuffer = vertexLayout.getLocationAndSize(key);

        glEnableVertexAttribArray(glAttribLocation.location);
        glVertexAttribPointer(glAttribLocation.location, glAttribLocation.elementCount, glAttribLocation.elementType, GL_FALSE, vertexLayout.getElementSize(), ptr + locationAndSizeInBuffer.location);
        glError = glGetError();
        PGE_ASSERT(glError == GL_NO_ERROR, "Failed to set vertex attribute (filepath: " + filepath.str() + "; attrib: " + String::fromInt(key.hash) + ")");
    }

    for (auto& it : vertexShaderConstants) {
        it.second.setUniform();
    }

    for (auto& it : fragmentShaderConstants) {
        it.second.setUniform();
    }

    for (auto& it : samplerConstants) {
        it.second.setUniform();
    }
}

void ShaderOGL3::unbindGLAttribs() {
    graphics->takeGlContext();

    for (const auto& kvp : glVertexAttribLocations) {
        const GlAttribLocation& glAttribLocation = kvp.second;
        glDisableVertexAttribArray(glAttribLocation.location);
    }
}

Shader::Constant* ShaderOGL3::getVertexShaderConstant(const String& name) {
    auto it = vertexShaderConstants.find(name);
    if (it != vertexShaderConstants.end()) {
        return &it->second;
    }
    return nullptr;
}

Shader::Constant* ShaderOGL3::getFragmentShaderConstant(const String& name) {
    auto it = fragmentShaderConstants.find(name);
    if (it != fragmentShaderConstants.end()) {
        return &it->second;
    }
    return nullptr;
}

ShaderOGL3::ConstantOGL3::ConstantOGL3(GraphicsOGL3* gfx, GLint glLoc, GLenum glTyp, int glArrSz, StructuredData& data, const String::Key& dk) : dataBuffer(data) {
    graphics = gfx;
    glLocation = glLoc;
    glType = glTyp;
    glArraySize = glArrSz;
    dataKey = dk;
}

void ShaderOGL3::ConstantOGL3::setValue(const Matrix4x4f& value) {
    dataBuffer.setValue(0, dataKey, value);
}

void ShaderOGL3::ConstantOGL3::setValue(const Vector2f& value) {
    dataBuffer.setValue(0, dataKey, value);
}

void ShaderOGL3::ConstantOGL3::setValue(const Vector3f& value) {
    dataBuffer.setValue(0, dataKey, value);
}

void ShaderOGL3::ConstantOGL3::setValue(const Vector4f& value) {
    dataBuffer.setValue(0, dataKey, value);
}

void ShaderOGL3::ConstantOGL3::setValue(const Color& value) {
    dataBuffer.setValue(0, dataKey, value);
}

void ShaderOGL3::ConstantOGL3::setValue(float value) {
    dataBuffer.setValue(0, dataKey, value);
}

void ShaderOGL3::ConstantOGL3::setValue(u32 value) {
    dataBuffer.setValue(0, dataKey, value);
}

void ShaderOGL3::ConstantOGL3::setUniform() {
    GLuint glError = GL_NO_ERROR;

    graphics->takeGlContext();
    const void* dataPtr = dataBuffer.getData().data() + dataBuffer.getLayout().getLocationAndSize(dataKey).location;
    const GLfloat* dataPtrF = (GLfloat*)dataPtr;
    const GLint* dataPtrI = (GLint*)dataPtr;
    const GLuint* dataPtrU = (GLuint*)dataPtr;
    switch (glType) {
        case GL_FLOAT_MAT4: {
            glUniformMatrix4fv(glLocation, 1, GL_FALSE, dataPtrF);
        } break;
        case GL_FLOAT_VEC2: {
            glUniform2f(glLocation, *dataPtrF, *(dataPtrF + 1));
        } break;
        case GL_FLOAT_VEC3: {
            glUniform3f(glLocation, *dataPtrF, *(dataPtrF + 1), *(dataPtrF + 2));
        } break;
        case GL_FLOAT_VEC4: {
            glUniform4f(glLocation, *dataPtrF, *(dataPtrF + 1), *(dataPtrF + 2), *(dataPtrF + 3));
        } break;
        case GL_FLOAT: {
            glUniform1f(glLocation, *dataPtrF);
        } break;
        case GL_INT: {
            glUniform1i(glLocation, *dataPtrI);
        } break;
        case GL_UNSIGNED_INT: {
            glUniform1ui(glLocation, *dataPtrU);
        } break;
    }

    glError = glGetError();
    PGE_ASSERT(glError == GL_NO_ERROR, "Failed to set uniform value (GLERROR: " + String::fromInt(glError) +")");
}

ShaderOGL3::GlAttribLocation::GlAttribLocation(GLint loc, GLenum elemType, int elemCount) {
    location = loc; elementType = elemType; elementCount = elemCount;
}

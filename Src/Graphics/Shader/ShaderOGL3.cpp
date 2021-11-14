#include "../GraphicsOGL3.h"

using namespace PGE;

static const String RT_NAME = "_PGE_INTERNAL_YFLIP";

ShaderOGL3::ShaderOGL3(Graphics& gfx, const FilePath& path) : Shader(path), resourceManager(gfx), graphics((GraphicsOGL3&)gfx) {
    graphics.takeGlContext();

    String vertexSource = (path + "vertex.glsl").readText();
    PGE_ASSERT(!vertexSource.isEmpty(), "Failed to find vertex.glsl (filepath: " + path.str() + ")");
    glVertexShader = resourceManager.addNewResource<GLShader>(GL_VERTEX_SHADER, vertexSource);

    String fragmentSource = (path + "fragment.glsl").readText();
    PGE_ASSERT(!fragmentSource.isEmpty(), "Failed to find fragment shader (filepath: " + path.str() + ")");
    glFragmentShader = resourceManager.addNewResource<GLShader>(GL_FRAGMENT_SHADER, fragmentSource);

    glShaderProgram = resourceManager.addNewResource<GLProgram>(std::vector{ glVertexShader.get(), glFragmentShader.get() });

    // TODO: Revisit the multiple passes needed here.
    extractVertexUniforms(vertexSource);

    extractVertexAttributes(vertexSource);

    extractFragmentUniforms(fragmentSource);

    extractFragmentOutputs(fragmentSource);

    Shader::Constant& rtConstant = *getVertexShaderConstant(RT_NAME);
    rtConstant.setValue(1.f);
    graphics.addRenderTargetFlag(rtConstant);
}

ShaderOGL3::~ShaderOGL3() {
    graphics.removeRenderTargetFlag(*getVertexShaderConstant(RT_NAME));
}

void ShaderOGL3::extractVertexUniforms(const String& vertexSource) {
    std::vector<ParsedShaderVar> vertexUniforms;
    extractShaderVars(vertexSource, "uniform", vertexUniforms);
    std::vector<StructuredData::ElemLayout::Entry> layoutEntries;
    for (int i = 0; i < (int)vertexUniforms.size(); i++) {
        int arrSize = 1; //TODO: add array support
        GLenum glType = parsedTypeToGlType(vertexUniforms[i].type);
        int byteSize = glSizeToByteSize(glType, arrSize);
        layoutEntries.emplace_back(vertexUniforms[i].name, byteSize);
        vertexShaderConstants.emplace(
            vertexUniforms[i].name,
            ConstantOGL3(
                graphics,
                glGetUniformLocation(glShaderProgram, vertexUniforms[i].name.cstr()),
                glType,
                arrSize,
                vertexUniformData,
                String::Key(vertexUniforms[i].name)
            )
        );
    }
    vertexUniformData = StructuredData(StructuredData::ElemLayout(layoutEntries), 1);
}

void ShaderOGL3::extractVertexAttributes(const String& vertexSource) {
    const String vertexInputPrefix = "vertexInput_";

    std::vector<ParsedShaderVar> parsedAttribs;
    extractShaderVars(vertexSource, "in", parsedAttribs);

    std::vector<StructuredData::ElemLayout::Entry> layoutEntries;
    for (int i = 0; i < (int)parsedAttribs.size(); i++) {
        String attrName = parsedAttribs[i].name;
        String sanitizedAttrName = attrName;
        if (sanitizedAttrName.findFirst(vertexInputPrefix) == sanitizedAttrName.begin()) {
            sanitizedAttrName = sanitizedAttrName.substr(vertexInputPrefix.length());
        }

        GLenum attrType = parsedTypeToGlType(parsedAttribs[i].type);
        GLenum attrElemType; int attrElemCount;
        decomposeGlType(attrType, attrElemType, attrElemCount);

        layoutEntries.emplace_back(sanitizedAttrName, glSizeToByteSize(attrElemType, attrElemCount));
        glVertexAttribLocations.emplace(
            String::Key(sanitizedAttrName),
            GlAttribLocation(
                glGetAttribLocation(glShaderProgram, attrName.cstr()),
                attrElemType,
                attrElemCount
            )
        );
    }

    vertexLayout = StructuredData::ElemLayout(layoutEntries);
}

void ShaderOGL3::extractFragmentUniforms(const String& fragmentSource) {
    std::vector<ParsedShaderVar> fragmentUniforms;
    extractShaderVars(fragmentSource, "uniform", fragmentUniforms);
    std::vector<StructuredData::ElemLayout::Entry> layoutEntries;
    for (int i = 0; i < (int)fragmentUniforms.size(); i++) {
        int arrSize = 1; //TODO: add array support
        GLenum glType = parsedTypeToGlType(fragmentUniforms[i].type);
        int byteSize = glSizeToByteSize(glType, arrSize);
        layoutEntries.emplace_back(fragmentUniforms[i].name, byteSize);
    }
    fragmentUniformData = StructuredData(StructuredData::ElemLayout(layoutEntries), 1);

    for (int i = 0; i < (int)fragmentUniforms.size(); i++) {
        int arrSize = 1; //TODO: add array support
        ConstantOGL3 constant(
            graphics,
            glGetUniformLocation(glShaderProgram, fragmentUniforms[i].name.cstr()),
            parsedTypeToGlType(fragmentUniforms[i].type),
            arrSize,
            fragmentUniformData,
            String::Key(fragmentUniforms[i].name)
        );
        if (fragmentUniforms[i].type.equals("sampler2D")) {
            constant.setValue((u32)samplerConstants.size());
            samplerConstants.emplace(fragmentUniforms[i].name, constant);
        } else {
            fragmentShaderConstants.emplace(fragmentUniforms[i].name, constant);
        }
    }

    for (int i = 0; i < (int)samplerConstants.size(); i++) {
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
    String line;
    String varStr = varKind + " ";
    for (char16 ch : src) {
        if (ch != '\r' && ch != '\n') {
            line += ch;
        } else {
            int minLen = varStr.length() < line.length() ? varStr.length() : line.length();
            if (line.substr(0, minLen).equals(varStr)) {
                bool typeHasBeenRead = false;
                ParsedShaderVar var;
                auto it = line.begin() + varStr.length();
                while (it != line.end()) {
                    char16 lineCh = *it;
                    if (lineCh == ' ') {
                        if (typeHasBeenRead && var.name.length() > 0) {
                            break;
                        }
                        typeHasBeenRead = true;
                    } else {
                        if (lineCh == ';' || lineCh == '\r' || lineCh == '\n') {
                            break;
                        } else {
                            if (typeHasBeenRead) {
                                var.name += lineCh;
                            } else {
                                var.type += lineCh;
                            }
                        }
                    }
                    it++;
                }
                varList.emplace_back(var);
            }
            line = String();
        }
    }
}

int ShaderOGL3::glSizeToByteSize(GLenum type, int size) const {
    int retVal = size;
    switch (type) {
        case GL_INT: {
            retVal *= sizeof(GLint);
        } break;
        case GL_UNSIGNED_INT: {
            retVal *= sizeof(GLuint);
        } break;
        case GL_FLOAT: {
            retVal *= sizeof(GLfloat);
        } break;
        case GL_FLOAT_VEC2: {
            retVal *= sizeof(GLfloat) * 2;
        } break;
        case GL_FLOAT_VEC3: {
            retVal *= sizeof(GLfloat) * 3;
        } break;
        case GL_FLOAT_VEC4: {
            retVal *= sizeof(GLfloat) * 4;
        } break;
        case GL_FLOAT_MAT4: {
            retVal *= sizeof(GLfloat) * 4 * 4;
        } break;
        default: {
            throw PGE_CREATE_EX("Unsupported OpenGL datatype: " + String::from(type));
        } break;
    }
    return retVal;
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
    } else if (parsedType.equals("sampler2D")) {
        return GL_INT;
    } else if (parsedType.equals("uint")) {
        return GL_UNSIGNED_INT;
    } else {
        throw PGE_CREATE_EX("Unsupported GLSL datatype: "+parsedType);
    }
}

void ShaderOGL3::decomposeGlType(GLenum compositeType, GLenum& elemType, int& elemCount) {
    switch (compositeType) {
        case GL_INT: {
            elemType = GL_INT; elemCount = 1;
        } break;
        case GL_UNSIGNED_INT: {
            elemType = GL_UNSIGNED_INT; elemCount = 1;
        } break;
        case GL_FLOAT: {
            elemType = GL_FLOAT; elemCount = 1;
        } break;
        case GL_FLOAT_VEC2: {
            elemType = GL_FLOAT; elemCount = 2;
        } break;
        case GL_FLOAT_VEC3: {
            elemType = GL_FLOAT; elemCount = 3;
        } break;
        case GL_FLOAT_VEC4: {
            elemType = GL_FLOAT; elemCount = 4;
        } break;
        case GL_FLOAT_MAT4: {
            elemType = GL_FLOAT; elemCount = 4 * 4;
        } break;
        default: {
            throw PGE_CREATE_EX("Unsupported OpenGL datatype: " + String::from(compositeType));
        }
    }
}

void ShaderOGL3::useShader() {
    GLuint glError = GL_NO_ERROR;

    graphics.takeGlContext();

    glUseProgram(glShaderProgram);

    byte* ptr = nullptr;
    for (const auto& [key, glAttribLocation] : glVertexAttribLocations) {
        const StructuredData::ElemLayout::LocationAndSize& locationAndSizeInBuffer = vertexLayout.getLocationAndSize(key);

        glEnableVertexAttribArray(glAttribLocation.location);
        glVertexAttribPointer(glAttribLocation.location, glAttribLocation.elementCount, glAttribLocation.elementType, GL_FALSE, vertexLayout.getElementSize(), ptr + locationAndSizeInBuffer.location);
        glError = glGetError();
        PGE_ASSERT(glError == GL_NO_ERROR, "Failed to set vertex attribute (filepath: " + filepath.str() + "; attrib: " + String::hexFromInt(key.hash) + ")");
    }

    for (auto& [_, constant] : vertexShaderConstants) {
        constant.setUniform();
    }

    for (auto& [_, constant] : fragmentShaderConstants) {
        constant.setUniform();
    }

    for (auto& [_, constant] : samplerConstants) {
        constant.setUniform();
    }
}

void ShaderOGL3::unbindGLAttribs() {
    graphics.takeGlContext();

    for (const auto& [_, glAttribLocation] : glVertexAttribLocations) {
        glDisableVertexAttribArray(glAttribLocation.location);
    }
}

Shader::Constant* ShaderOGL3::getVertexShaderConstant(const String& name) {
    auto it = vertexShaderConstants.find(name);
    if (it == vertexShaderConstants.end()) { return nullptr; }
    return &it->second;
}

Shader::Constant* ShaderOGL3::getFragmentShaderConstant(const String& name) {
    auto it = fragmentShaderConstants.find(name);
    if (it == fragmentShaderConstants.end()) { return nullptr; }
    return &it->second;
}

ShaderOGL3::ConstantOGL3::ConstantOGL3(GraphicsOGL3& gfx, GLint glLoc, GLenum glTyp, int glArrSz, StructuredData& data, const String::Key& dk) : dataBuffer(data), graphics(gfx) {
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

    graphics.takeGlContext();
    const void* dataPtr = dataBuffer.getData() + dataBuffer.getLayout().getLocationAndSize(dataKey).location;
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
    PGE_ASSERT(glError == GL_NO_ERROR, "Failed to set uniform value (GLERROR: " + String::from(glError) +")");
}

ShaderOGL3::GlAttribLocation::GlAttribLocation(GLint loc, GLenum elemType, int elemCount) {
    location = loc; elementType = elemType; elementCount = elemCount;
}

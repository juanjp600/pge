#include "../GraphicsOGL3.h"

using namespace PGE;

static const String RT_NAME = "_PGE_INTERNAL_YFLIP";

ShaderOGL3::ShaderOGL3(Graphics& gfx, const FilePath& path) : Shader(path), resourceManager(gfx), graphics((GraphicsOGL3&)gfx) {
    graphics.takeGlContext();

    String vertexSource = (path + "vertex.glsl").readText();
    asrt(!vertexSource.isEmpty(), "Failed to find vertex.glsl (filepath: " + path.str() + ")");
    glVertexShader = resourceManager.addNewResource<GLShader>(GL_VERTEX_SHADER, vertexSource);

    String fragmentSource = (path + "fragment.glsl").readText();
    asrt(!fragmentSource.isEmpty(), "Failed to find fragment shader (filepath: " + path.str() + ")");
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
    int vertexUniformSize = 0;
    for (const ParsedShaderVar& var : vertexUniforms) {
        int arrSize = 1; //TODO: add array support
        GLenum glType = parsedTypeToGlType(var.type);
        vertexShaderConstants.emplace(
            var.name,
            ConstantOGL3(
                graphics,
                glGetUniformLocation(glShaderProgram, var.name.cstr()),
                glType,
                arrSize,
                vertexUniformData,
                vertexUniformSize // Acts as offset here
            )
        );
        vertexUniformSize += glSizeToByteSize(glType, arrSize);
    }
    vertexUniformData = std::make_unique<byte[]>(vertexUniformSize);
}

void ShaderOGL3::extractVertexAttributes(const String& vertexSource) {
    const String vertexInputPrefix = "vertexInput_";

    std::vector<ParsedShaderVar> parsedAttribs;
    extractShaderVars(vertexSource, "in", parsedAttribs);

    std::vector<StructuredData::ElemLayout::Entry> layoutEntries;
    for (size_t i : Range(parsedAttribs.size())) {
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
    int fragmentUniformSize = 0;
    for (const ParsedShaderVar& var : fragmentUniforms) {
        int arrSize = 1; //TODO: add array support
        fragmentUniformSize += glSizeToByteSize(parsedTypeToGlType(var.type), arrSize);
    }

    fragmentUniformData = std::make_unique<byte[]>(fragmentUniformSize);

    fragmentUniformSize = 0;
    for (const ParsedShaderVar& var : fragmentUniforms) {
        int arrSize = 1; //TODO: add array support
        GLenum glType = parsedTypeToGlType(var.type);

        ConstantOGL3 constant(
            graphics,
            glGetUniformLocation(glShaderProgram, var.name.cstr()),
            glType,
            arrSize,
            fragmentUniformData,
            fragmentUniformSize // Offset
        );

        fragmentUniformSize += glSizeToByteSize(glType, arrSize);

        if (var.type.equals("sampler2D")) {
            constant.setValue((u32)samplerConstants.size());
            samplerConstants.emplace(var.name, constant);
        } else {
            fragmentShaderConstants.emplace(var.name, constant);
        }
    }

    textureCount = (int)samplerConstants.size();
}

void ShaderOGL3::extractFragmentOutputs(const String fragmentSource) {
    std::vector<ParsedShaderVar> fragmentOutputs;
    extractShaderVars(fragmentSource, "out", fragmentOutputs);
    for (GLuint i : Range((GLuint)fragmentOutputs.size())) {
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
            throw Exception("Unsupported OpenGL datatype: " + String::from(type));
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
        throw Exception("Unsupported GLSL datatype: "+parsedType);
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
            throw Exception("Unsupported OpenGL datatype: " + String::from(compositeType));
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
        asrt(glError == GL_NO_ERROR, "Failed to set vertex attribute (filepath: " + filepath.str() + "; attrib: " + String::hexFromInt(key.hash) + ")");
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

ShaderOGL3::ConstantOGL3::ConstantOGL3(GraphicsOGL3& gfx, GLint glLoc, GLenum glType, int glArrSz,
    std::unique_ptr<byte[]>& data, int offset)
    : graphics(gfx), glLocation(glLoc), glType(glType), glArraySize(glArrSz),
    data(data), offset(offset) { }

void ShaderOGL3::ConstantOGL3::setValueInternal(const std::span<byte>& value) {
    memcpy(&data[offset], value.data(), value.size());
}

void ShaderOGL3::ConstantOGL3::setUniform() {
    graphics.takeGlContext();
    const byte* dataPtr = &data[offset];
    const GLfloat* dataPtrF = (GLfloat*)dataPtr;
    switch (glType) {
        case GL_FLOAT_MAT4: {
            glUniformMatrix4fv(glLocation, 1, GL_FALSE, dataPtrF);
        } break;
        case GL_FLOAT_VEC2: {
            glUniform2f(glLocation, dataPtrF[0], dataPtrF[1]);
        } break;
        case GL_FLOAT_VEC3: {
            glUniform3f(glLocation, dataPtrF[0], dataPtrF[1], dataPtrF[2]);
        } break;
        case GL_FLOAT_VEC4: {
            glUniform4f(glLocation, dataPtrF[0], dataPtrF[1], dataPtrF[2], dataPtrF[3]);
        } break;
        case GL_FLOAT: {
            glUniform1f(glLocation, *dataPtrF);
        } break;
        case GL_INT: {
            glUniform1i(glLocation, *(GLint*)dataPtr);
        } break;
        case GL_UNSIGNED_INT: {
            glUniform1ui(glLocation, *(GLuint*)dataPtr);
        } break;
    }

    GLuint glError = glGetError();
    asrt(glError == GL_NO_ERROR, "Failed to set uniform value (GLERROR: " + String::from(glError) +")");
}

ShaderOGL3::GlAttribLocation::GlAttribLocation(GLint loc, GLenum elemType, int elemCount) {
    location = loc; elementType = elemType; elementCount = elemCount;
}

#include "ShaderOGL3.h"

#include <PGE/Graphics/Graphics.h>
#include <PGE/Exception/Exception.h>
#include "../GraphicsOGL3.h"

using namespace PGE;

ShaderOGL3::ShaderOGL3(Graphics* gfx, const FilePath& path) : resourceManager(gfx, 3) {
    graphics = gfx;
    ((GraphicsOGL3*)graphics)->takeGlContext();

    filepath = path;

    std::vector<byte> vertexFile = (path + "vertex.glsl").readBytes();
    PGE_ASSERT(!vertexFile.empty(), "Failed to find vertex.glsl (filepath: " + path.str() + ")");
    vertexFile.push_back(0);
    String vertexSource((char*)vertexFile.data());
    std::vector<ShaderVar> vertexUniforms;
    extractShaderVars(vertexSource, "uniform", vertexUniforms);
    glVertexShader = resourceManager.addNewResource<GLShader>(GL_VERTEX_SHADER, vertexSource);

    std::vector<byte> fragmentFile = (path + "fragment.glsl").readBytes();
    PGE_ASSERT(!fragmentFile.empty(), "Failed to find fragment shader (filepath: " + path.str() + ")");
    fragmentFile.push_back(0);
    String fragmentSource((char*)fragmentFile.data());
    std::vector<ShaderVar> fragmentUniforms;
    extractShaderVars(fragmentSource, "uniform", fragmentUniforms);
    glFragmentShader = resourceManager.addNewResource<GLShader>(GL_FRAGMENT_SHADER, fragmentSource);

    glShaderProgram = resourceManager.addNewResource<GLProgram>(std::vector{ glVertexShader.get(), glFragmentShader.get() });

    for (int i = 0; i < (int)vertexUniforms.size(); i++) {
        vertexShaderConstants.emplace(vertexUniforms[i].name, ConstantOGL3(graphics,glGetUniformLocation(glShaderProgram, vertexUniforms[i].name.cstr())));
    }

    for (int i = 0; i < (int)fragmentUniforms.size(); i++) {
        ConstantOGL3 constant = ConstantOGL3(graphics,glGetUniformLocation(glShaderProgram, fragmentUniforms[i].name.cstr()));
        if (fragmentUniforms[i].type.equals("sampler2D")) {
            constant.setValue((int)samplerConstants.size());
            samplerConstants.emplace(fragmentUniforms[i].name, constant);
        } else {
            fragmentShaderConstants.emplace(fragmentUniforms[i].name, constant);
        }
    }

    std::vector<ShaderVar> vertexInput;
    extractShaderVars(vertexSource,"in",vertexInput);
    stride = 0;
    for (int i = 0; i < (int)vertexInput.size(); i++) {
        VertexAttrib attrib;
        attrib.name = vertexInput[i].name;
        attrib.location = glGetAttribLocation(glShaderProgram, vertexInput[i].name.cstr());
        if (vertexInput[i].type.equals("float")) {
            attrib.size = 1;
            attrib.type = GL_FLOAT;
            stride += sizeof(GLfloat)*1;
        } else if (vertexInput[i].type.equals("vec2")) {
            attrib.size = 2;
            attrib.type = GL_FLOAT;
            stride += sizeof(GLfloat)*2;
        } else if (vertexInput[i].type.equals("vec3")) {
            attrib.size = 3;
            attrib.type = GL_FLOAT;
            stride += sizeof(GLfloat)*3;
        } else if (vertexInput[i].type.equals("vec4")) {
            attrib.size = 4;
            attrib.type = GL_FLOAT;
            stride += sizeof(GLfloat)*4;
        } else if (vertexInput[i].type.equals("int")) {
            attrib.size = 1;
            attrib.type = GL_INT;
            stride += sizeof(GLint)*1;
        }
        vertexAttribs.push_back(attrib);
        vertexInputElems.push_back(attrib.name);
    }

    std::vector<ShaderVar> fragmentOutputs;
    extractShaderVars(fragmentSource,"out",fragmentOutputs);
    for (int i = 0; i < (int)fragmentOutputs.size(); i++) {
        glBindFragDataLocation(glShaderProgram,i,fragmentOutputs[i].name.cstr());
    }
}

const std::vector<String>& ShaderOGL3::getVertexInputElems() const {
    return vertexInputElems;
}

void ShaderOGL3::useShader() {
    GLuint glError = GL_NO_ERROR;

    ((GraphicsOGL3*)graphics)->takeGlContext();

    glUseProgram(glShaderProgram);

    byte* ptr = nullptr;
    for (int i = 0; i < (int)vertexAttribs.size(); i++) {
        glEnableVertexAttribArray(vertexAttribs[i].location);
        switch (vertexAttribs[i].type) {
            case GL_FLOAT: {
                glVertexAttribPointer(vertexAttribs[i].location,vertexAttribs[i].size,GL_FLOAT,GL_FALSE,stride,(void*)ptr);
                ptr+=sizeof(GLfloat)*vertexAttribs[i].size;
            } break;
            case GL_INT: {
                glVertexAttribPointer(vertexAttribs[i].location,vertexAttribs[i].size,GL_INT,GL_FALSE,stride,(void*)ptr);
                ptr+=sizeof(GLint)*vertexAttribs[i].size;
            } break;
        }
        glError = glGetError();
        PGE_ASSERT(glError == GL_NO_ERROR, "Failed to set vertex attribute (filepath: " + filepath.str() + "; attrib: " + vertexAttribs[i].name + ")");
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
    ((GraphicsOGL3*)graphics)->takeGlContext();

    for (int i = 0; i < (int)vertexAttribs.size(); i++) {
        glDisableVertexAttribArray(vertexAttribs[i].location);
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

void ShaderOGL3::extractShaderVars(const String& src,const String& varKind,std::vector<ShaderVar>& varList) {
    String line = "";
    String varStr = varKind + " ";
    for (char32_t chr : src) {
        if (chr!='\r' && chr!='\n') {
            line += chr;
        } else {
            if (line.substr(0, varStr.length() < line.length() ? varStr.length() : line.length()).equals(varStr)) {
                bool readType = false;
                ShaderVar var;
                var.type = "";
                var.name = "";
                auto it = line.begin() + (varStr.length() - 1);
                while (++it != line.end()) {
                    char32_t chr = *it;
                    if (chr==' ') {
                        if (readType && var.name.length()>0) {
                            break;
                        }
                        readType = true;
                    } else {
                        if (chr==';' || chr=='\r' || chr=='\n') {
                            break;
                        } else {
                            if (readType) {
                                var.name += chr;
                            } else {
                                var.type += chr;
                            }
                        }
                    }
                }
                varList.push_back(var);
            }
            line = "";
        }
    }
}

ShaderOGL3::ConstantOGL3::ConstantOGL3(Graphics* gfx, int loc) {
    graphics = gfx;
    location = loc;
}

void ShaderOGL3::ConstantOGL3::setValue(const Matrix4x4f& value) {
    val.matrixVal = value; valueType = ValueType::MATRIX;
}

void ShaderOGL3::ConstantOGL3::setValue(const Vector2f& value) {
    val.vector2fVal = value; valueType = ValueType::VECTOR2F;
}

void ShaderOGL3::ConstantOGL3::setValue(const Vector3f& value) {
    val.vector3fVal = value; valueType = ValueType::VECTOR3F;
}

void ShaderOGL3::ConstantOGL3::setValue(const Vector4f& value) {
    val.vector4fVal = value; valueType = ValueType::VECTOR4F;
}

void ShaderOGL3::ConstantOGL3::setValue(const Color& value) {
    val.colorVal = value; valueType = ValueType::COLOR;
}

void ShaderOGL3::ConstantOGL3::setValue(float value) {
    val.floatVal = value; valueType = ValueType::FLOAT;
}

void ShaderOGL3::ConstantOGL3::setValue(int value) {
    val.intVal = value; valueType = ValueType::INT;
}

void ShaderOGL3::ConstantOGL3::setUniform() {
    GLuint glError = GL_NO_ERROR;

    ((GraphicsOGL3*)graphics)->takeGlContext();
    switch (valueType) {
        case ValueType::MATRIX: {
            glUniformMatrix4fv(location, 1, GL_FALSE, val.matrixVal[0]);
        } break;
        case ValueType::VECTOR2F: {
            glUniform2f(location,val.vector2fVal.x,val.vector2fVal.y);
        } break;
        case ValueType::VECTOR3F: {
            glUniform3f(location,val.vector3fVal.x,val.vector3fVal.y,val.vector3fVal.z);
        } break;
        case ValueType::VECTOR4F: {
            glUniform4f(location,val.vector4fVal.x,val.vector4fVal.y,val.vector4fVal.z,val.vector4fVal.w);
        } break;
        case ValueType::COLOR: {
            glUniform4f(location,val.colorVal.red,val.colorVal.green,val.colorVal.blue,val.colorVal.alpha);
        } break;
        case ValueType::FLOAT: {
            glUniform1f(location,val.floatVal);
        } break;
        case ValueType::INT: {
            glUniform1i(location,val.intVal);
        } break;
    }

    glError = glGetError();
    PGE_ASSERT(glError == GL_NO_ERROR, "Failed to set uniform value (GLERROR: " + String::fromInt(glError) +")");
}

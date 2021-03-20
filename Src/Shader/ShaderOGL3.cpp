#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsOGL3.h"
#include "ShaderOGL3.h"
#include <Exception/Exception.h>
#include <Misc/FileUtil.h>

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

using namespace PGE;

ShaderOGL3::ShaderOGL3(Graphics* gfx,const FilePath& path) {
    graphics = gfx;
    ((GraphicsOGL3*)graphics)->takeGlContext();

    filepath = path;

    std::vector<uint8_t> vertexFile; FileUtil::readBytes(path + "vertex.glsl", vertexFile);
    if (vertexFile.empty()) {
        throwException("ShaderOGL3", "Failed to find vertex.glsl. (filepath: " + path.str() + ")");
    }
    vertexFile.push_back(0);
    const char* cstr = (char*)&vertexFile[0];
    String vertexSource = String(cstr);

    std::vector<ShaderVar> vertexUniforms;
    extractShaderVars(vertexSource,"uniform",vertexUniforms);

    int errorCode = 0;
    glVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(glVertexShader,1,&cstr,nullptr);
    glCompileShader(glVertexShader);

    String errorStr;
    char* errorCStr = new char[512];
    GLsizei len = 0;
    glGetShaderInfoLog(glVertexShader, 512, &len, errorCStr);
    errorStr = String(errorCStr);

    glGetShaderiv(glVertexShader, GL_COMPILE_STATUS, &errorCode);
    if (errorCode != GL_TRUE || errorStr.length() > 0) {
        delete[] errorCStr;
        throwException("ShaderOGL3", "Failed to create vertex shader. (filepath: " + path.str() + ")\n" + errorStr);
    }

    std::vector<uint8_t> fragmentFile; FileUtil::readBytes(path + "fragment.glsl", fragmentFile);
    if (fragmentFile.empty()) {
        throwException("ShaderOGL3", "Failed to find fragment.glsl. (filepath: " + path.str() + ")");
    }
    fragmentFile.push_back(0);
    cstr = (char*)&fragmentFile[0];
    String fragmentSource = String(cstr);

    std::vector<ShaderVar> fragmentUniforms;
    extractShaderVars(fragmentSource,"uniform",fragmentUniforms);

    glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(glFragmentShader,1,&cstr,nullptr);
    glCompileShader(glFragmentShader);
    glGetShaderInfoLog(glFragmentShader, 512, &len, errorCStr);
    errorStr = String(errorCStr);

    glGetShaderiv(glFragmentShader, GL_COMPILE_STATUS, &errorCode);
    if (errorCode != GL_TRUE || errorStr.length() > 0) {
        delete[] errorCStr;
        throwException("ShaderOGL3", "Failed to create fragment shader. (filepath: " + path.str() + ")\n" + errorStr);
    }
    delete[] errorCStr;

    // TODO: Memleak! Shaders are not being detached. Fixed on Vulkan branch.
    glShaderProgram = glCreateProgram();
    glAttachShader(glShaderProgram,glVertexShader);
    glAttachShader(glShaderProgram,glFragmentShader);

    glLinkProgram(glShaderProgram);

    for (int i = 0; i < (int)vertexUniforms.size(); i++) {
        vertexShaderConstants.push_back(ConstantOGL3(graphics,vertexUniforms[i].name,glGetUniformLocation(glShaderProgram, vertexUniforms[i].name.cstr())));
    }

    for (int i = 0; i < (int)fragmentUniforms.size(); i++) {
        ConstantOGL3 constant = ConstantOGL3(graphics,fragmentUniforms[i].name,glGetUniformLocation(glShaderProgram, fragmentUniforms[i].name.cstr()));
        if (fragmentUniforms[i].type.equals("sampler2D")) {
            constant.setValue((int)samplerConstants.size());
            samplerConstants.push_back(constant);
        } else {
            fragmentShaderConstants.push_back(constant);
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

    uint8_t* ptr = nullptr;
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
        if (glError != GL_NO_ERROR) {
            throwException("useShader", "Failed to set vertex attribute. (Attrib: " + vertexAttribs[i].name + ", filepath: " + filepath.str() + ")");
        }
    }

    for (int i = 0; i < (int)vertexShaderConstants.size(); i++) {
        vertexShaderConstants[i].setUniform();
    }

    for (int i = 0; i < (int)fragmentShaderConstants.size(); i++) {
        fragmentShaderConstants[i].setUniform();
    }

    for (int i = 0; i < (int)samplerConstants.size(); i++) {
        samplerConstants[i].setUniform();
    }
}

void ShaderOGL3::unbindGLAttribs() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    for (int i = 0; i < (int)vertexAttribs.size(); i++) {
        glDisableVertexAttribArray(vertexAttribs[i].location);
    }
}

ShaderOGL3::~ShaderOGL3() {
    cleanup();
}

void ShaderOGL3::throwException(String func, String details) {
    cleanup();
    throw Exception("ShaderOGL3::" + func, details);
}

void ShaderOGL3::cleanup() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    if (glShaderProgram != 0) { glDeleteProgram(glShaderProgram); }
    if (glFragmentShader != 0) { glDeleteShader(glFragmentShader); }
    if (glVertexShader != 0) { glDeleteShader(glVertexShader); }
}

Shader::Constant* ShaderOGL3::getVertexShaderConstant(String name) {
    for (int i = 0; i < (int)vertexShaderConstants.size(); i++) {
        if (vertexShaderConstants[i].getName().equals(name)) {
            return &vertexShaderConstants[i];
        }
    }
    return nullptr;
}

Shader::Constant* ShaderOGL3::getFragmentShaderConstant(String name) {
    for (int i = 0; i < (int)fragmentShaderConstants.size(); i++) {
        if (fragmentShaderConstants[i].getName().equals(name)) {
            return &fragmentShaderConstants[i];
        }
    }
    return nullptr;
}

void ShaderOGL3::extractShaderVars(const String& src,String varKind,std::vector<ShaderVar>& varList) {
    String line = "";
    varKind = String(varKind," ");
    for (int i=0; i < src.length(); i++) {
        PGE::wchar chr = src.charAt(i);
        if (chr!='\r' && chr!='\n') {
            line = String(line,chr);
        } else {
            if (line.substr(0,varKind.length()).equals(varKind)) {
                bool readType = false;
                ShaderVar var;
                var.type = "";
                var.name = "";
                for (int j=varKind.length();j<line.length();j++) {
                    chr = line.charAt(j);
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
                                var.name = String(var.name,chr);
                            } else {
                                var.type = String(var.type,chr);
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

ShaderOGL3::ConstantOGL3::ConstantOGL3(Graphics* gfx,String nm, int loc) {
    graphics = gfx;
    name = nm;
    location = loc;
}

ShaderOGL3::ConstantOGL3::Value::Value() {
    matrixVal = Matrix4x4f();
}

void ShaderOGL3::ConstantOGL3::setValue(Matrix4x4f value) {
    val.matrixVal = value; valueType = VALUE_TYPE::MATRIX;
}

void ShaderOGL3::ConstantOGL3::setValue(Vector2f value) {
    val.vector2fVal = value; valueType = VALUE_TYPE::VECTOR2F;
}

void ShaderOGL3::ConstantOGL3::setValue(Vector3f value) {
    val.vector3fVal = value; valueType = VALUE_TYPE::VECTOR3F;
}

void ShaderOGL3::ConstantOGL3::setValue(Vector4f value) {
    val.vector4fVal = value; valueType = VALUE_TYPE::VECTOR4F;
}

void ShaderOGL3::ConstantOGL3::setValue(Color value) {
    val.colorVal = value; valueType = VALUE_TYPE::COLOR;
}

void ShaderOGL3::ConstantOGL3::setValue(float value) {
    val.floatVal = value; valueType = VALUE_TYPE::FLOAT;
}

void ShaderOGL3::ConstantOGL3::setValue(int value) {
    val.intVal = value; valueType = VALUE_TYPE::INT;
}

void ShaderOGL3::ConstantOGL3::setUniform() {
    GLuint glError = GL_NO_ERROR;

    ((GraphicsOGL3*)graphics)->takeGlContext();
    switch (valueType) {
        case VALUE_TYPE::MATRIX: {
            glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)val.matrixVal.elements);
        } break;
        case VALUE_TYPE::VECTOR2F: {
            glUniform2f(location,val.vector2fVal.x,val.vector2fVal.y);
        } break;
        case VALUE_TYPE::VECTOR3F: {
            glUniform3f(location,val.vector3fVal.x,val.vector3fVal.y,val.vector3fVal.z);
        } break;
        case VALUE_TYPE::VECTOR4F: {
            glUniform4f(location,val.vector4fVal.x,val.vector4fVal.y,val.vector4fVal.z,val.vector4fVal.w);
        } break;
        case VALUE_TYPE::COLOR: {
            glUniform4f(location,val.colorVal.red,val.colorVal.green,val.colorVal.blue,val.colorVal.alpha);
        } break;
        case VALUE_TYPE::FLOAT: {
            glUniform1f(location,val.floatVal);
        } break;
        case VALUE_TYPE::INT: {
            glUniform1i(location,val.intVal);
        } break;
    }

    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throwException("setUniform", "Failed to set uniform value. (Constant Name: " + getName() + ")");
    }
}

String ShaderOGL3::ConstantOGL3::getName() const {
    return name;
}

void ShaderOGL3::ConstantOGL3::throwException(String func, String details) {
    throw Exception("ConstantOGL3::" + func, details);
}

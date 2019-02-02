#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsOGL3.h"
#include "ShaderOGL3.h"

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif
#include <fstream>

using namespace PGE;

Shader* Shader::load(Graphics* gfx,const String& path) {
    return new ShaderOGL3(gfx,path);
}

ShaderOGL3::ShaderOGL3(Graphics* gfx,const String& path) {
    graphics = gfx;
    ((GraphicsOGL3*)graphics)->takeGlContext();

    filepath = path;

    String vertexSource = "";
    std::ifstream vertexSourceFile; vertexSourceFile.open(String(path,"vertex.glsl").cstr());
    char* buf = new char[512];
    while (!vertexSourceFile.eof()) {
        vertexSourceFile.read(buf,511);
        buf[vertexSourceFile.gcount()]='\0';
        vertexSource = String(vertexSource,buf);
    }
    delete[] buf;

    std::vector<ShaderVar> vertexUniforms;
    extractShaderVars(vertexSource,"uniform",vertexUniforms);

    const char* cstr = vertexSource.cstr();


    glVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(glVertexShader,1,&cstr,nullptr);
    glCompileShader(glVertexShader);

    String fragmentSource = "";
    std::ifstream fragmentSourceFile; fragmentSourceFile.open(String(path,"fragment.glsl").cstr());
    buf = new char[512];
    while (!fragmentSourceFile.eof()) {
        fragmentSourceFile.read(buf,511);
        buf[fragmentSourceFile.gcount()]='\0';
        fragmentSource = String(fragmentSource,buf);
    }
    delete[] buf;

    std::vector<ShaderVar> fragmentUniforms;
    extractShaderVars(fragmentSource,"uniform",fragmentUniforms);

    cstr = fragmentSource.cstr();

    glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(glFragmentShader,1,&cstr,nullptr);
    glCompileShader(glFragmentShader);

    glShaderProgram = glCreateProgram();
    glAttachShader(glShaderProgram,glVertexShader);
    glAttachShader(glShaderProgram,glFragmentShader);

    glLinkProgram(glShaderProgram);

    for (int i=0;i<vertexUniforms.size();i++) {
        vertexShaderConstants.push_back(ConstantOGL3(graphics,vertexUniforms[i].name,glGetUniformLocation(glShaderProgram, vertexUniforms[i].name.cstr())));
    }

    int samplerCount = 0;
    for (int i=0;i<fragmentUniforms.size();i++) {
        if (fragmentUniforms[i].type.equals("sampler2D")) {
            glUniform1i(glGetUniformLocation(glShaderProgram, fragmentUniforms[i].name.cstr()), samplerCount);
            samplerCount++;
        } else {
            fragmentShaderConstants.push_back(ConstantOGL3(graphics,fragmentUniforms[i].name,glGetUniformLocation(glShaderProgram, fragmentUniforms[i].name.cstr())));
        }
    }
    SDL_Log("Sampler count: %d\n",samplerCount);

    std::vector<ShaderVar> vertexInput;
    extractShaderVars(vertexSource,"in",vertexInput);
    int offset = 0;
    for (int i=0;i<vertexInput.size();i++) {
        VertexAttrib attrib;
        attrib.name = vertexInput[i].name;
        attrib.location = glGetAttribLocation(glShaderProgram, vertexInput[i].name.cstr());
        if (vertexInput[i].type.equals("float")) {
            attrib.size = 1;
            attrib.type = GL_FLOAT;
        } else if (vertexInput[i].type.equals("vec2")) {
            attrib.size = 2;
            attrib.type = GL_FLOAT;
        } else if (vertexInput[i].type.equals("vec3")) {
            attrib.size = 3;
            attrib.type = GL_FLOAT;
        } else if (vertexInput[i].type.equals("vec4")) {
            attrib.size = 4;
            attrib.type = GL_FLOAT;
        } else if (vertexInput[i].type.equals("int")) {
            attrib.size = 1;
            attrib.type = GL_INT;
        }
        vertexAttribs.push_back(attrib);
    }

    std::vector<ShaderVar> fragmentOutputs;
    extractShaderVars(fragmentSource,"out",fragmentOutputs);
    for (int i=0;i<fragmentOutputs.size();i++) {
        glBindFragDataLocation(glShaderProgram,i,fragmentOutputs[i].name.cstr());
    }
}

void ShaderOGL3::bindGLAttribs() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    uint8_t* ptr = nullptr;
    for (int i=0;i<vertexAttribs.size();i++) {
        glEnableVertexAttribArray(vertexAttribs[i].location);
        switch (vertexAttribs[i].type) {
            case GL_FLOAT: {
                glVertexAttribPointer(vertexAttribs[i].location,vertexAttribs[i].size,GL_FLOAT,GL_FALSE,0,(void*)ptr);
                ptr+=sizeof(GLfloat)*vertexAttribs[i].size;
            } break;
            case GL_INT: {
                glVertexAttribPointer(vertexAttribs[i].location,vertexAttribs[i].size,GL_INT,GL_FALSE,0,(void*)ptr);
                ptr+=sizeof(GLint)*vertexAttribs[i].size;
            } break;
        }
    }
}

void ShaderOGL3::unbindGLAttribs() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    for (int i=0;i<vertexAttribs.size();i++) {
        glDisableVertexAttribArray(vertexAttribs[i].location);
    }}

ShaderOGL3::~ShaderOGL3() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    glDeleteProgram(glShaderProgram);
    glDeleteShader(glFragmentShader);
    glDeleteShader(glVertexShader);
}

Shader::Constant* ShaderOGL3::getVertexShaderConstant(String name) {
    for (int i=0;i<vertexShaderConstants.size();i++) {
        if (vertexShaderConstants[i].getName().equals(name)) {
            return &vertexShaderConstants[i];
        }    }
    return nullptr;
}

Shader::Constant* ShaderOGL3::getFragmentShaderConstant(String name) {
    for (int i=0;i<fragmentShaderConstants.size();i++) {
        if (fragmentShaderConstants[i].getName().equals(name)) {
            return &fragmentShaderConstants[i];
        }    }
    return nullptr;
}

void ShaderOGL3::extractShaderVars(const String& src,String varKind,std::vector<ShaderVar>& varList) {
    String line = "";
    varKind = String(varKind," ");
    for (int i=0;i<src.size();i++) {
        char chr = src.charAt(i);
        if (chr!='\r' && chr!='\n') {
            line = String(line,chr);
        } else {
            if (line.substr(0,varKind.size()).equals(varKind)) {
                bool readType = false;
                ShaderVar var;
                var.type = "";
                var.name = "";
                for (int j=varKind.size();j<line.size();j++) {
                    chr = line.charAt(j);
                    if (chr==' ') {
                        if (readType && var.name.size()>0) {
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
                SDL_Log("Var name: %s\n",var.name.cstr());
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
    SDL_Log("Constant info: %s %d",name.cstr(),loc);
}

void ShaderOGL3::ConstantOGL3::setValue(Matrix4x4f value) {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)value.elements);
}

void ShaderOGL3::ConstantOGL3::setValue(Vector3f value) {
    glUniform3f(location,value.x,value.y,value.z);
}

void ShaderOGL3::ConstantOGL3::setValue(float value) {
    glUniform1f(location,value);
}

void ShaderOGL3::ConstantOGL3::setValue(int value) {
    glUniform1i(location,value);
}

String ShaderOGL3::ConstantOGL3::getName() const {
    return name;
}

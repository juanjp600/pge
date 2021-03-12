#include "ShaderOGL3.h"

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

#include <spirv_reflect.h>

#include <Graphics/Graphics.h>
#include <Exception/Exception.h>
#include <Misc/FileUtil.h>
#include "../Graphics/GraphicsOGL3.h"

using namespace PGE;

ShaderOGL3::ShaderOGL3(Graphics* gfx,const FilePath& path) {
    graphics = gfx;
    ((GraphicsOGL3*)graphics)->takeGlContext();

    filepath = path;

    destructor.setPreop(new GraphicsOGL3::OpTakeContext((GraphicsOGL3*)gfx));

    glVertexShader = destructor.getReference<GLuint>([](const GLuint& i) { glDeleteShader(i); }, glCreateShader(GL_VERTEX_SHADER));
    glFragmentShader = destructor.getReference<GLuint>([](const GLuint& i) { glDeleteShader(i); }, glCreateShader(GL_FRAGMENT_SHADER));
    GLuint shaders[2] = { glVertexShader(), glFragmentShader() };

    int errorCode = GL_NO_ERROR;

    std::vector<uint8_t> shaderBinary = FileUtil::readBytes(path + "shader.spv");
    glShaderBinary(2, shaders, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBinary.data(), shaderBinary.size());
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        throw Exception("ShaderOGL3", "Failed to set shader binary. (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
    }
    SpvReflectShaderModule reflection; SpvReflectResult res = spvReflectCreateShaderModule(shaderBinary.size(), shaderBinary.data(), &reflection);

    glSpecializeShader(glVertexShader(), "VS", 0, nullptr, nullptr);

    GLsizei len;
    std::vector<char> error = std::vector<char>(512);
    glGetShaderInfoLog(glVertexShader(), 512, &len, error.data());
    String errorStr = String(error.data());

    glGetShaderiv(glVertexShader(), GL_COMPILE_STATUS, &errorCode);
    if (errorCode != GL_TRUE || errorStr.length() > 0) {
        throw Exception("ShaderOGL3", "Failed to create vertex shader. (filepath: " + path.str() + ")\n" + errorStr);
    }

    glSpecializeShader(glFragmentShader(), "PS", 0, nullptr, nullptr);

    glGetShaderInfoLog(glFragmentShader(), 512, &len, error.data());
    errorStr = String(error.data());

    glGetShaderiv(glFragmentShader(), GL_COMPILE_STATUS, &errorCode);
    if (errorCode != GL_TRUE || errorStr.length() > 0) {
        throw Exception("ShaderOGL3", "Failed to create fragment shader. (filepath: " + path.str() + ")\n" + errorStr);
    }

    glShaderProgram = destructor.getReference<GLuint, SmartRef<GLuint>, SmartRef<GLuint>>(glCreateProgram(), glVertexShader, glVertexShader,
        +[](const GLuint& p, SmartRef<GLuint> v, SmartRef<GLuint> f) { glDetachShader(p, v()); glDetachShader(p, f()); glDeleteProgram(p); });
    glAttachShader(glShaderProgram(),glVertexShader());
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        throw Exception("ShaderOGL3", "Failed to attach vertex shader. (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
    }
    glAttachShader(glShaderProgram(),glFragmentShader());
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        throw Exception("ShaderOGL3", "Failed to attach fragment shader. (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
    }

    glLinkProgram(glShaderProgram());
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        throw Exception("ShaderOGL3", "Failed to link shader program. (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
    }

    int constantSize = 0;
    if (reflection.push_constant_block_count > 0) {
        SpvReflectBlockVariable pushConstant = reflection.push_constant_blocks[0];
        for (int i = 0; i < pushConstant.member_count; i++) {
            constantSize += pushConstant.members[i].padded_size;
        }
        constantBuffer.resize(constantSize);
        for (int i = 0; i < pushConstant.member_count; i++) {
            SpvReflectBlockVariable var = pushConstant.members[i];
            String name = var.name;
            ConstantOGL3 constant = ConstantOGL3(name.substr(5), (void*)(constantBuffer.data() + var.absolute_offset));
            if (name.substr(0, 4) == "vert") {
                vertexShaderConstants.push_back(constant);
            } else {
                /*if (fragmentUniforms[i].type.equals("sampler2D")) {
                    constant.setValue((int)samplerConstants.size());
                    samplerConstants.push_back(constant);
                } else {*/
                fragmentShaderConstants.push_back(constant);
                //}
                // TODO: WTF.
                // "descriptor set" bullshit etc.
            }
        }
    }
    // TODO: We only need this when we actually have constants.
    uniformBufferObject = destructor.getReference<GLuint>([](const GLuint& i) { glDeleteBuffers(1, &i); }, 0);
    glGenBuffers(1, &uniformBufferObject);
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        throw Exception("ShaderOGL3", "Failed to generate uniformBufferObject. (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
    }

    glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferObject());
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        throw Exception("ShaderOGL3", "Failed to bind uniformBufferObject. (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
    }

    // TODO: Static or dynamic draw, who knows?
    glBufferData(GL_UNIFORM_BUFFER, constantBuffer.size(), constantBuffer.data(), GL_STATIC_DRAW);
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        throw Exception("ShaderOGL3", "Failed to set constantBuffer data. (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBufferObject());
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        throw Exception("ShaderOGL3", "Failed to bind uniformBufferObject buffer base. (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
    }

    stride = 0;
    for (int i=0; i < reflection.input_variable_count; i++) {
        SpvReflectInterfaceVariable* var = reflection.input_variables[i];
        VertexAttrib attrib;
        attrib.name = String(var->name).substr(6);
        attrib.location = var->location;
        errorCode = glGetError();
        if (errorCode != GL_NO_ERROR)
        {
            throw Exception("ShaderOGL3", "Failed to get "+attrib.name+" location. (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
        }
        switch (var->format) {
            case SPV_REFLECT_FORMAT_R32_SFLOAT: {
                attrib.size = 1;
                attrib.type = GL_FLOAT;
                stride += sizeof(GLfloat) * 1;
            } break;
            case SPV_REFLECT_FORMAT_R32G32_SFLOAT: {
                attrib.size = 2;
                attrib.type = GL_FLOAT;
                stride += sizeof(GLfloat) * 2;
            } break;
            case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT: {
                attrib.size = 3;
                attrib.type = GL_FLOAT;
                stride += sizeof(GLfloat) * 3;
            } break;
            case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT: {
                attrib.size = 4;
                attrib.type = GL_FLOAT;
                stride += sizeof(GLfloat) * 4;
            } break;
            case SPV_REFLECT_FORMAT_R32_SINT: {
                attrib.size = 1;
                attrib.type = GL_INT;
                stride += sizeof(GLint) * 1;
            } break;
        }
        vertexAttribs.push_back(attrib);
        vertexInputElems.push_back(attrib.name);
    }

    // TODO: Depth etc.
    for (int i = 0; i < reflection.output_variable_count; i++) {
        //if (reflection.output_variables[i]->built_in == -1) {
            glBindFragDataLocation(glShaderProgram(), i, reflection.output_variables[i]->name);
            errorCode = glGetError();
            if (errorCode != GL_NO_ERROR)
            {
                throw Exception("ShaderOGL3", "Failed to bind fragment data location "+String::fromInt(i)+". (filepath: " + path.str() + ", error code" + String::fromInt(errorCode) + ")");
            }
        //}
    }

    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR)
    {
        throw Exception("ShaderOGL3", "FFS");
    }
}

const std::vector<String>& ShaderOGL3::getVertexInputElems() const {
    return vertexInputElems;
}

void ShaderOGL3::useShader() {
    GLuint glError = GL_NO_ERROR;

    ((GraphicsOGL3*)graphics)->takeGlContext();


    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throw Exception("useShader", "FFS");
    }

    glUseProgram(glShaderProgram());
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throw Exception("useShader", "Failed to use shader program. (filepath: " + filepath.str() + ", error code: " + String::fromInt(glError) +")");
    }

    uint8_t* ptr = nullptr;
    for (int i=0;i<vertexAttribs.size();i++) {
        glEnableVertexAttribArray(vertexAttribs[i].location);
        glError = glGetError();
        if (glError != GL_NO_ERROR) {
            throw Exception("useShader", "Failed to enable vertex attribute. (Attrib: " + vertexAttribs[i].name + ", filepath: " + filepath.str() + ", error code: " + String::fromInt(glError) + ")");
        }
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
            throw Exception("useShader", "Failed to set vertex attribute. (Attrib: " + vertexAttribs[i].name + ", filepath: " + filepath.str() + ", error code: " + String::fromInt(glError) + ")");
        }
    }

    glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferObject());
    // TODO: Buffer data sensefully.
    glBufferSubData(GL_UNIFORM_BUFFER, 0, constantBuffer.size(), constantBuffer.data());
}

void ShaderOGL3::unbindGLAttribs() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    for (int i=0;i<vertexAttribs.size();i++) {
        glDisableVertexAttribArray(vertexAttribs[i].location);
    }
}

Shader::Constant* ShaderOGL3::getVertexShaderConstant(String name) {
    for (int i=0;i<vertexShaderConstants.size();i++) {
        if (vertexShaderConstants[i].getName().equals(name)) {
            return &vertexShaderConstants[i];
        }
    }
    return nullptr;
}

Shader::Constant* ShaderOGL3::getFragmentShaderConstant(String name) {
    for (int i=0;i<fragmentShaderConstants.size();i++) {
        if (fragmentShaderConstants[i].getName().equals(name)) {
            return &fragmentShaderConstants[i];
        }
    }
    return nullptr;
}

ShaderOGL3::ConstantOGL3::ConstantOGL3(String nm, void* loc) {
    name = nm;
    location = loc;
}

void ShaderOGL3::ConstantOGL3::setValue(Matrix4x4f value) {
    memcpy(location, value.elements, 4 * 4 * sizeof(float));
}

void ShaderOGL3::ConstantOGL3::setValue(Vector2f value) {
    float* floc = (float*)location;
    floc[0] = value.x;
    floc[1] = value.y;
}

void ShaderOGL3::ConstantOGL3::setValue(Vector3f value) {
    float* floc = (float*)location;
    floc[0] = value.x;
    floc[1] = value.y;
    floc[2] = value.z;
}

void ShaderOGL3::ConstantOGL3::setValue(Vector4f value) {
    float* floc = (float*)location;
    floc[0] = value.x;
    floc[1] = value.y;
    floc[2] = value.z;
    floc[3] = value.w;
}

void ShaderOGL3::ConstantOGL3::setValue(Color value) {
    float* floc = (float*)location;
    floc[0] = value.red;
    floc[1] = value.green;
    floc[2] = value.blue;
    floc[3] = value.alpha;
}

void ShaderOGL3::ConstantOGL3::setValue(float value) {
    *(float*)location = value;
}

void ShaderOGL3::ConstantOGL3::setValue(int value) {
    *(int*)location = value;
}

String ShaderOGL3::ConstantOGL3::getName() const {
    return name;
}

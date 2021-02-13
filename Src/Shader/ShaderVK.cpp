#include "ShaderVK.h"

#include <Exception/Exception.h>
#include <Misc/FileUtil.h>

using namespace PGE;

ShaderVK::ShaderVK(Graphics* gfx, const FilePath& path) {
    tempConstant = new ConstantVK();

    FileUtil::readBytes(path + "vert.spv");
}

ShaderVK::~ShaderVK() {
    
}

ShaderVK::~ShaderVK() {
    cleanup();
}

Shader::Constant* ShaderVK::getVertexShaderConstant(String name) {
    return tempConstant;
}

Shader::Constant* ShaderVK::getFragmentShaderConstant(String name) {
    return tempConstant;
}

void ShaderVK::cleanup() {

}

void ShaderVK::throwException(String func, String details) {
    cleanup();
    throw Exception("ShaderVK::" + func, details);
}

ShaderVK::ConstantVK::ConstantVK() {

}

void ShaderVK::ConstantVK::setValue(Matrix4x4f value) {

}

void ShaderVK::ConstantVK::setValue(Vector2f value) {

}

void ShaderVK::ConstantVK::setValue(Vector3f value) {

}

void ShaderVK::ConstantVK::setValue(Vector4f value) {

}

void ShaderVK::ConstantVK::setValue(Color value) {

}

void ShaderVK::ConstantVK::setValue(float value) {

}

void ShaderVK::ConstantVK::setValue(int value) {

}

void ShaderVK::ConstantVK::throwException(String function, String details) {

}

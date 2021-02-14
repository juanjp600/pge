#include "ShaderVK.h"

#include <Exception/Exception.h>
#include <Misc/FileUtil.h>

#include "../Graphics/GraphicsVK.h"

using namespace PGE;

ShaderVK::ShaderVK(Graphics* gfx, const FilePath& path) {
    tempConstant = new ConstantVK();

    device = ((GraphicsVK*)gfx)->getDevice();

    std::vector<uint8_t> vertexShaderFile = FileUtil::readBytes(path + "vert.spv");
    vk::ShaderModuleCreateInfo vertexShaderCreateInfo = vk::ShaderModuleCreateInfo({}, vertexShaderFile.size(), (uint32_t*)&vertexShaderFile[0]);
    vertexShader = device.createShaderModule(vertexShaderCreateInfo);

    std::vector<uint8_t> fragmentShaderFile = FileUtil::readBytes(path + "frag.spv");
    vk::ShaderModuleCreateInfo fragmentShaderCreateInfo = vk::ShaderModuleCreateInfo({}, fragmentShaderFile.size(), (uint32_t*)&fragmentShaderFile[0]);
    fragmentShader = device.createShaderModule(fragmentShaderCreateInfo);

    modules[0] = vertexShader;
    modules[1] = fragmentShader;

    vk::PipelineShaderStageCreateInfo vertexInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertexShader, "VS");
    vk::PipelineShaderStageCreateInfo fragmentInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragmentShader, "PS");
    shaderStageInfo[0] = vertexInfo;
    shaderStageInfo[1] = fragmentInfo;

    vertexInputInfo = vk::PipelineVertexInputStateCreateInfo({}, 0, nullptr, 0, nullptr);

    // TODO: Constants.
    vk::PipelineLayoutCreateInfo layoutInfo = vk::PipelineLayoutCreateInfo();
    layout = device.createPipelineLayout(layoutInfo);
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
    device.destroyShaderModule(vertexShader);
    device.destroyShaderModule(fragmentShader);
    device.destroyPipelineLayout(layout);
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

vk::ShaderModule* ShaderVK::getModules() {
    return &modules[0];
}

vk::PipelineShaderStageCreateInfo* ShaderVK::getShaderStageInfo() {
    return &shaderStageInfo[0];
}

vk::PipelineVertexInputStateCreateInfo* ShaderVK::getVertexInputInfo() {
    return &vertexInputInfo;
}

vk::PipelineLayout* ShaderVK::getLayout() {
    return &layout;
}

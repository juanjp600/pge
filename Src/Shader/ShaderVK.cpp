#include "ShaderVK.h"

#include <Exception/Exception.h>
#include <Misc/FileUtil.h>

#include "../Graphics/GraphicsVK.h"

#include <spirv_reflect.h>

using namespace PGE;

void ShaderVK::clearMap(const std::map<long long, PGE::ShaderVK::ConstantVK*>& m) {
    for (std::pair<long long, ConstantVK*> constant : m) {
        delete constant.second;
    }
}

ShaderVK::ShaderVK(Graphics* gfx, const FilePath& path) {
    graphics = (GraphicsVK*)gfx;
    vk::Device device = ((GraphicsVK*)gfx)->getDevice();

    destructor.setPreop(new GraphicsVK::OpDeviceIdle(device));

    // Shader.
    std::vector<uint8_t> shaderFile = FileUtil::readBytes(path + "shader.spv");
    vk::ShaderModuleCreateInfo shaderCreateInfo = vk::ShaderModuleCreateInfo({}, shaderFile.size(), (uint32_t*)&shaderFile[0]);
    vkShader = destructor.getReference(device.createShaderModule(shaderCreateInfo), device,
        +[](const vk::ShaderModule& s, vk::Device d) { d.destroy(s); });

    // Reflect.
    SpvReflectShaderModule reflection; SpvReflectResult res = spvReflectCreateShaderModule(shaderFile.size(), &shaderFile[0], &reflection);
    // Vertex input.
    vertexInputNames.resize(reflection.input_variable_count);
    vertexInputAttributes = std::vector<vk::VertexInputAttributeDescription>(reflection.input_variable_count);
    vertexStride = 0;
    for (int i = 0; i < reflection.input_variable_count; i++) {
        // Strip the "input.".
        vertexInputNames[i] = String(reflection.input_variables[0][i].name).substr(6);
        vertexInputAttributes[i] = vk::VertexInputAttributeDescription(reflection.input_variables[0][i].location, 0, (vk::Format)reflection.input_variables[0][i].format, vertexStride);
        // TODO: Better way to get vertex input size?
        vertexStride += 4 * (reflection.input_variables[0][i].numeric.vector.component_count > 0 ? reflection.input_variables[0][i].numeric.vector.component_count : 1);
    }

    // Push constants.
    // Members are supposed to begin with the vertex constants and not meant to be mixed.
    vertexConstantMap = destructor.getReference<std::map<long long, ShaderVK::ConstantVK*>>(clearMap);
    fragmentConstantMap = destructor.getReference<std::map<long long, ShaderVK::ConstantVK*>>(clearMap);

    std::vector<vk::PushConstantRange> ranges;
    if (reflection.push_constant_block_count != 0) {
        if (reflection.push_constant_block_count > 1) {
        //    throw Exception("ShaderVK::ShaderVK", "Too many push constants (" + String::fromInt(reflection.push_constant_block_count) + ") in shader " + path.cstr());
        }
        auto lol = reflection.push_constant_blocks[1];
        SpvReflectBlockVariable pushConstant = reflection.push_constant_blocks[0];
        if (String(pushConstant.name) != "vulkanConstants") {
            throw Exception("ShaderVK::ShaderVK", String("Invalid push constant named \"") + pushConstant.name + "\".");
        }

        ranges.reserve(2);
        int fragmentOffset;
        for (int j = 0; j < pushConstant.member_count; j++) {
            String name = pushConstant.members[j].name;
            if (name.substr(0, 4) == "vert") {
                vertexConstantMap().emplace(name.substr(5).getHashCode(), new PGE::ShaderVK::ConstantVK(graphics, this, vk::ShaderStageFlagBits::eVertex, pushConstant.members[j].absolute_offset));
            } else {
                if (fragmentConstantMap().size() == 0) {
                    fragmentOffset = pushConstant.members[j].absolute_offset;
                }
                fragmentConstantMap().emplace(name.substr(5).getHashCode(), new PGE::ShaderVK::ConstantVK(graphics, this, vk::ShaderStageFlagBits::eFragment, pushConstant.members[j].absolute_offset));
            }
        }
        if (!vertexConstantMap().empty()) { ranges.push_back(vk::PushConstantRange({ vk::ShaderStageFlagBits::eVertex }, 0, fragmentOffset)); }
        if (!fragmentConstantMap().empty()) { ranges.push_back(vk::PushConstantRange({ vk::ShaderStageFlagBits::eFragment }, fragmentOffset, pushConstant.padded_size - fragmentOffset)); }
    }
    
    vk::PipelineLayoutCreateInfo layoutInfo({}, 0, nullptr, ranges.size(), ranges.empty() ? nullptr : &ranges[0]);
    layout = destructor.getReference(device.createPipelineLayout(layoutInfo), device,
        +[](const vk::PipelineLayout& l, vk::Device d) { d.destroy(l); });
    
    spvReflectDestroyShaderModule(&reflection);

    // Both constructor values need to remain in memory!
    vertexInputBinding = vk::VertexInputBindingDescription(0, vertexStride, vk::VertexInputRate::eVertex);
    vertexInputInfo = vk::PipelineVertexInputStateCreateInfo({}, 1, &vertexInputBinding, vertexInputAttributes.size(), &vertexInputAttributes[0]);

    vk::PipelineShaderStageCreateInfo vertexInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vkShader(), "VS");
    vk::PipelineShaderStageCreateInfo fragmentInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, vkShader(), "PS");
    shaderStageInfo[0] = vertexInfo;
    shaderStageInfo[1] = fragmentInfo;
}

Shader::Constant* ShaderVK::getVertexShaderConstant(String name) {
    return vertexConstantMap()[name.getHashCode()];
}

Shader::Constant* ShaderVK::getFragmentShaderConstant(String name) {
    return fragmentConstantMap()[name.getHashCode()];
}

ShaderVK::ConstantVK::ConstantVK(Graphics* gfx, ShaderVK* she, vk::ShaderStageFlags stg, int off) {
    graphics = gfx;
    shader = she;
    stage = stg;
    offset = off;
}

void ShaderVK::ConstantVK::setValue(Matrix4x4f value) {
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(*shader->getLayout(), stage, offset, 4 * 4 * sizeof(float), &value.elements[0][0]);
}

void ShaderVK::ConstantVK::setValue(Vector2f value) {
    float val[] = { value.x, value.y };
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(*shader->getLayout(), stage, offset, 2 * sizeof(float), val);
}

void ShaderVK::ConstantVK::setValue(Vector3f value) {
    float val[] = { value.x, value.y, value.z };
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(*shader->getLayout(), stage, offset, 3 * sizeof(float), val);
}

void ShaderVK::ConstantVK::setValue(Vector4f value) {
    float val[] = { value.x, value.y, value.z, value.w };
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(*shader->getLayout(), stage, offset, 4 * sizeof(float), val);
}

void ShaderVK::ConstantVK::setValue(Color value) {
    float val[] = { value.red, value.green, value.blue, value.alpha };
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(*shader->getLayout(), stage, offset, 4 * sizeof(float), val);
}

void ShaderVK::ConstantVK::setValue(float value) {
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(*shader->getLayout(), stage, offset, sizeof(float), &value);
}

void ShaderVK::ConstantVK::setValue(int value) {
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(*shader->getLayout(), stage, offset, sizeof(int), &value);
}

int ShaderVK::getVertexStride() const {
    return vertexStride;
}

std::vector<String> ShaderVK::getVertexInputNames() const {
    return vertexInputNames;
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

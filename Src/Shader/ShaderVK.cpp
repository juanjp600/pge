#include "ShaderVK.h"

#include <Exception/Exception.h>
#include <Misc/FileUtil.h>

#include "../Graphics/GraphicsVK.h"

#include <spirv_reflect.h>

using namespace PGE;

ShaderVK::ShaderVK(Graphics* gfx, const FilePath& path) {
    graphics = (GraphicsVK*)gfx;
    device = ((GraphicsVK*)gfx)->getDevice();

    // Shader.
    std::vector<uint8_t> shaderFile = FileUtil::readBytes(path + "shader.spv");
    vk::ShaderModuleCreateInfo shaderCreateInfo = vk::ShaderModuleCreateInfo({}, shaderFile.size(), (uint32_t*)&shaderFile[0]);
    vkShader = device.createShaderModule(shaderCreateInfo);

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
    vk::PushConstantRange pushConstants[2];
    // Push constants.
    for (int i = 0; i < reflection.push_constant_block_count; i++) {
        SpvReflectBlockVariable var = reflection.push_constant_blocks[i];
        vk::ShaderStageFlags stage;
        if (!strcmp(var.name, "cbMatrices")) {
            stage = vk::ShaderStageFlagBits::eVertex;
        } else if (!strcmp(var.name, "cbFragment")) {
            stage = vk::ShaderStageFlagBits::eFragment;
        } else {
            throw Exception("ShaderVK", "Invalid push constant name.");
        }
        pushConstants[i] = vk::PushConstantRange({ stage }, var.offset, var.size);
    }

    vk::PipelineLayoutCreateInfo layoutInfo({}, 0, nullptr, 2, pushConstants);
    layout = device.createPipelineLayout(layoutInfo);
    
    // TODO: Bug in shader compilation with identical push constant blocks.
    for (int i = 0; i < reflection.push_constant_block_count; i++) {
        for (int j = 0; j < reflection.push_constant_blocks[i].member_count; j++) {
            SpvReflectBlockVariable var = reflection.push_constant_blocks[i].members[j];
            std::map<long long, ConstantVK*>* map;
            if (pushConstants[i].stageFlags == vk::ShaderStageFlagBits::eVertex) {
                map = &vertexConstantMap;
            } else {
                map = &fragmentConstantMap;
            }
            map->emplace(String(var.name).getHashCode(), new ConstantVK(graphics, layout, pushConstants[i].stageFlags, var.offset));
        }
    }
    
    spvReflectDestroyShaderModule(&reflection);

    // Both constructor values need to remain in memory!
    vertexInputBinding = vk::VertexInputBindingDescription(0, vertexStride, vk::VertexInputRate::eVertex);
    vertexInputInfo = vk::PipelineVertexInputStateCreateInfo({}, 1, &vertexInputBinding, vertexInputAttributes.size(), &vertexInputAttributes[0]);

    vk::PipelineShaderStageCreateInfo vertexInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vkShader, "VS");
    vk::PipelineShaderStageCreateInfo fragmentInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, vkShader, "PS");
    shaderStageInfo[0] = vertexInfo;
    shaderStageInfo[1] = fragmentInfo;
}

Shader::Constant* ShaderVK::getVertexShaderConstant(String name) {
    return vertexConstantMap[name.getHashCode()];
}

Shader::Constant* ShaderVK::getFragmentShaderConstant(String name) {
    return fragmentConstantMap[name.getHashCode()];
}

/*void ShaderVK::cleanup() {
    for (std::pair<long long, ConstantVK*> constant : vertexConstantMap) {
        delete constant.second;
    }
    for (std::pair<long long, ConstantVK*> constant : fragmentConstantMap) {
        delete constant.second;
    }
    device.destroyShaderModule(vkShader);
    device.destroyPipelineLayout(layout);
}TODO*/

ShaderVK::ConstantVK::ConstantVK(Graphics* gfx, vk::PipelineLayout lay, vk::ShaderStageFlags stg, int off) {
    graphics = gfx;
    layout = lay;
    stage = stg;
    offset = off;
}

void ShaderVK::ConstantVK::setValue(Matrix4x4f value) {
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(layout, stage, offset, 4 * 4 * sizeof(float), &value.elements[0][0]);
}

void ShaderVK::ConstantVK::setValue(Vector2f value) {
    float val[] = { value.x, value.y };
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(layout, stage, offset, 2 * sizeof(float), val);
}

void ShaderVK::ConstantVK::setValue(Vector3f value) {
    float val[] = { value.x, value.y, value.z };
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(layout, stage, offset, 3 * sizeof(float), val);
}

void ShaderVK::ConstantVK::setValue(Vector4f value) {
    float val[] = { value.x, value.y, value.z, value.w };
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(layout, stage, offset, 4 * sizeof(float), val);
}

void ShaderVK::ConstantVK::setValue(Color value) {
    float val[] = { value.red, value.green, value.blue, value.alpha };
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(layout, stage, offset, 4 * sizeof(float), val);
}

void ShaderVK::ConstantVK::setValue(float value) {
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(layout, stage, offset, sizeof(float), &value);
}

void ShaderVK::ConstantVK::setValue(int value) {
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(layout, stage, offset, sizeof(int), &value);
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

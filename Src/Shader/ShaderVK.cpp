#include "ShaderVK.h"

#include <Exception/Exception.h>
#include <Misc/FileUtil.h>

#include "../Graphics/GraphicsVK.h"

#include <spirv_reflect.h>

using namespace PGE;

ShaderVK::ShaderVK(Graphics* gfx, const FilePath& path) : resourceManager(gfx, 2) {
    graphics = (GraphicsVK*)gfx;
    vk::Device device = ((GraphicsVK*)gfx)->getDevice();

    // Shader.
    std::vector<uint8_t> shaderBinary; FileUtil::readBytes(path + "shader.spv", shaderBinary);
    vkShader = VKShader::createRef(resourceManager, device, shaderBinary);

    // Reflect.
    SpvReflectShaderModule reflection; SpvReflectResult res = spvReflectCreateShaderModule(shaderBinary.size(), shaderBinary.data(), &reflection);
    // Vertex input.
    vertexInputNames.reserve(reflection.input_variable_count);
    vertexInputAttributes.reserve(reflection.input_variable_count);
    vertexStride = 0;
    for (int i = 0; i < (int)reflection.input_variable_count; i++) {
        // We don't want built in variables.
        if (reflection.input_variables[0][i].built_in != -1) {
            continue;
        }
        // Strip the "input.".
        vertexInputNames.push_back(String(reflection.input_variables[0][i].name).substr(6));
        vertexInputAttributes.push_back(vk::VertexInputAttributeDescription(reflection.input_variables[0][i].location, 0, (vk::Format)reflection.input_variables[0][i].format, vertexStride));
        // TODO: Better way to get vertex input size?
        vertexStride += 4 * (reflection.input_variables[0][i].numeric.vector.component_count > 0 ? reflection.input_variables[0][i].numeric.vector.component_count : 1);
    }

    // Push constants.
    // Members are supposed to begin with the vertex constants and not meant to be mixed.
    std::vector<vk::PushConstantRange> ranges;
    if (reflection.push_constant_block_count != 0) {
        if (reflection.push_constant_block_count > 1) {
        //    throw Exception("ShaderVK::ShaderVK", "Too many push constants (" + String::fromInt(reflection.push_constant_block_count) + ") in shader " + path.cstr());
        } // TODO: Clean.
        auto lol = reflection.push_constant_blocks[1];
        SpvReflectBlockVariable pushConstant = reflection.push_constant_blocks[0];
        String blockName = pushConstant.name;
        __ASSERT(blockName == "vulkanConstants", "Invalid push constant (\"" + blockName + "\")");

        ranges.reserve(2);
        int fragmentOffset;
        for (int j = 0; j < (int)pushConstant.member_count; j++) {
            String name = pushConstant.members[j].name;
            if (name.substr(0, 4) == "vert") {
                vertexConstantMap.emplace(name.substr(5).getHashCode(), std::make_unique<PGE::ShaderVK::ConstantVK>(graphics, this, vk::ShaderStageFlagBits::eVertex, pushConstant.members[j].absolute_offset));
            } else {
                if (fragmentConstantMap.size() == 0) {
                    fragmentOffset = pushConstant.members[j].absolute_offset;
                }
                fragmentConstantMap.emplace(name.substr(5).getHashCode(), std::make_unique<PGE::ShaderVK::ConstantVK>(graphics, this, vk::ShaderStageFlagBits::eFragment, pushConstant.members[j].absolute_offset));
            }
        }
        if (!vertexConstantMap.empty()) { ranges.push_back(vk::PushConstantRange({ vk::ShaderStageFlagBits::eVertex }, 0, fragmentOffset)); }
        if (!fragmentConstantMap.empty()) { ranges.push_back(vk::PushConstantRange({ vk::ShaderStageFlagBits::eFragment }, fragmentOffset, pushConstant.padded_size - fragmentOffset)); }
    }

    layout = VKPipelineLayout::createRef(resourceManager, device, ranges);
    
    spvReflectDestroyShaderModule(&reflection);

    // Both constructor values need to remain in memory!
    vertexInputBinding = vk::VertexInputBindingDescription(0, vertexStride, vk::VertexInputRate::eVertex);
    vertexInputInfo = vk::PipelineVertexInputStateCreateInfo({}, 1, &vertexInputBinding, (uint32_t)vertexInputAttributes.size(), vertexInputAttributes.data());

    vk::PipelineShaderStageCreateInfo vertexInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vkShader(), "VS");
    vk::PipelineShaderStageCreateInfo fragmentInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, vkShader(), "PS");
    shaderStageInfo[0] = vertexInfo;
    shaderStageInfo[1] = fragmentInfo;
}

Shader::Constant* ShaderVK::getVertexShaderConstant(const String& name) {
    auto it = vertexConstantMap.find(name.getHashCode());
    if (it == vertexConstantMap.end()) {
        return nullptr;
    } else {
        return it->second.get();
    }
}

Shader::Constant* ShaderVK::getFragmentShaderConstant(const String& name) {
    auto it = fragmentConstantMap.find(name.getHashCode());
    if (it == fragmentConstantMap.end()) {
        return nullptr;
    } else {
        return it->second.get();
    }
}

ShaderVK::ConstantVK::ConstantVK(Graphics* gfx, ShaderVK* she, vk::ShaderStageFlags stg, int off) {
    graphics = gfx;
    shader = she;
    stage = stg;
    offset = off;
}

void ShaderVK::ConstantVK::setValue(Matrix4x4f value) {
    ((GraphicsVK*)graphics)->getCurrentCommandBuffer().pushConstants(*shader->getLayout(), stage, offset, 4 * 4 * sizeof(float), value.elements);
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
    return shaderStageInfo;
}

vk::PipelineVertexInputStateCreateInfo* ShaderVK::getVertexInputInfo() {
    return &vertexInputInfo;
}

// TODO: Why.
const vk::PipelineLayout* ShaderVK::getLayout() {
    return &layout;
}

#include "ShaderVK.h"

#include <Exception/Exception.h>

#include "../Graphics/GraphicsVK.h"

#include <spirv_reflect.h>

using namespace PGE;

ShaderVK::ShaderVK(Graphics* gfx, const FilePath& path) : resourceManager(gfx, 2) {
    graphics = (GraphicsVK*)gfx;
    vk::Device device = ((GraphicsVK*)gfx)->getDevice();

    // Shader.
    std::vector<uint8_t> shaderBinary; (path + "shader.spv").readBytes(shaderBinary);
    vkShader = resourceManager.addNewResource<VKShader>(device, shaderBinary);

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
        PGE_ASSERT(blockName == "vulkanConstants", "Invalid push constant (\"" + blockName + "\")");

        ranges.reserve(2);
        int fragmentOffset = pushConstant.padded_size;
        for (int j = 0; j < (int)pushConstant.member_count; j++) {
            String name = pushConstant.members[j].name;
            if (name.substr(0, 4) == "vert") {
                vertexConstantMap.emplace(name.substr(5), ConstantVK(this, vk::ShaderStageFlagBits::eVertex, pushConstant.members[j].absolute_offset));
            } else {
                if (fragmentConstantMap.size() == 0) {
                    fragmentOffset = pushConstant.members[j].absolute_offset;
                }
                fragmentConstantMap.emplace(name.substr(5), ConstantVK(this, vk::ShaderStageFlagBits::eFragment, pushConstant.members[j].absolute_offset));
            }
        }
        if (!vertexConstantMap.empty()) { ranges.push_back(vk::PushConstantRange({ vk::ShaderStageFlagBits::eVertex }, 0, fragmentOffset)); }
        if (!fragmentConstantMap.empty()) { ranges.push_back(vk::PushConstantRange({ vk::ShaderStageFlagBits::eFragment }, fragmentOffset, pushConstant.padded_size - fragmentOffset)); }
    }

    layout = resourceManager.addNewResource<VKPipelineLayout>(device, ranges);
    
    spvReflectDestroyShaderModule(&reflection);

    // Both constructor values need to remain in memory!
    vertexInputBinding = vk::VertexInputBindingDescription(0, vertexStride, vk::VertexInputRate::eVertex);
    vertexInputInfo = vk::PipelineVertexInputStateCreateInfo({}, 1, &vertexInputBinding, (uint32_t)vertexInputAttributes.size(), vertexInputAttributes.data());

    vk::PipelineShaderStageCreateInfo vertexInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vkShader, "VS");
    vk::PipelineShaderStageCreateInfo fragmentInfo = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, vkShader, "PS");
    shaderStageInfo[0] = vertexInfo;
    shaderStageInfo[1] = fragmentInfo;
}

Shader::Constant* ShaderVK::getVertexShaderConstant(const String& name) {
    auto it = vertexConstantMap.find(name);
    if (it == vertexConstantMap.end()) {
        return nullptr;
    } else {
        return &it->second;
    }
}

Shader::Constant* ShaderVK::getFragmentShaderConstant(const String& name) {
    auto it = fragmentConstantMap.find(name);
    if (it == fragmentConstantMap.end()) {
        return nullptr;
    } else {
        return &it->second;
    }
}

static int lololol;

void ShaderVK::pushConstants() {
    for (ConstantVK* c : updatedConstants) {
        c->push((GraphicsVK*)graphics);
    }
    // TODO: Why.
    //updatedConstants.clear();
}

ShaderVK::ConstantVK::ConstantVK(ShaderVK* she, vk::ShaderStageFlags stg, int off) {
    shader = she;
    stage = stg;
    offset = off;
}

void ShaderVK::ConstantVK::setValue(const Matrix4x4f& value) {
    val.matrixVal = value; valueType = Type::MATRIX;
    shader->updatedConstants.emplace(this);
}

void ShaderVK::ConstantVK::setValue(const Vector2f& value) {
    val.vector2fVal = value; valueType = Type::VECTOR2F;
    shader->updatedConstants.emplace(this);
}

void ShaderVK::ConstantVK::setValue(const Vector3f& value) {
    val.vector3fVal = value; valueType = Type::VECTOR3F;
    shader->updatedConstants.emplace(this);
}

void ShaderVK::ConstantVK::setValue(const Vector4f& value) {
    val.vector4fVal = value; valueType = Type::VECTOR4F;
    shader->updatedConstants.emplace(this);
}

void ShaderVK::ConstantVK::setValue(const Color& value) {
    val.colorVal = value; valueType = Type::COLOR;
    shader->updatedConstants.emplace(this);
}

void ShaderVK::ConstantVK::setValue(float value) {
    val.floatVal = value; valueType = Type::FLOAT;
    shader->updatedConstants.emplace(this);
}

void ShaderVK::ConstantVK::setValue(int value) {
    val.intVal = value; valueType = Type::INT;
    shader->updatedConstants.emplace(this);
}

void ShaderVK::ConstantVK::push(GraphicsVK* gfx) {
    switch (valueType) {
        case Type::MATRIX: {
            // TODO: Look into matrix order bullshit, I hate my life.
            gfx->getCurrentCommandBuffer().pushConstants(shader->getLayout(), stage, offset, 4 * 4 * sizeof(float), val.matrixVal.transpose().elements);
        } break;
        case Type::COLOR: {
            float value[] = { val.colorVal.red, val.colorVal.green, val.colorVal.blue, val.colorVal.alpha };
            gfx->getCurrentCommandBuffer().pushConstants(shader->getLayout(), stage, offset, 4 * sizeof(float), value);
        } break;
        case Type::FLOAT: {
            gfx->getCurrentCommandBuffer().pushConstants(shader->getLayout(), stage, offset, sizeof(float), &val.floatVal);
        } break;
        case Type::INT: {
            gfx->getCurrentCommandBuffer().pushConstants(shader->getLayout(), stage, offset, sizeof(int), &val.intVal);
        } break;
        case Type::VECTOR2F: {
            float value[] = { val.vector2fVal.x, val.vector2fVal.y };
            gfx->getCurrentCommandBuffer().pushConstants(shader->getLayout(), stage, offset, 2 * sizeof(float), value);
        } break;
        case Type::VECTOR3F: {
            float value[] = { val.vector3fVal.x, val.vector3fVal.y, val.vector3fVal.z };
            gfx->getCurrentCommandBuffer().pushConstants(shader->getLayout(), stage, offset, 3 * sizeof(float), value);
        } break;
        case Type::VECTOR4F: {
            float value[] = { val.vector4fVal.x, val.vector4fVal.y, val.vector4fVal.z, val.vector4fVal.w };
            gfx->getCurrentCommandBuffer().pushConstants(shader->getLayout(), stage, offset, 4 * sizeof(float), value);
        } break;
    }
}

int ShaderVK::getVertexStride() const {
    return vertexStride;
}

const std::vector<String>& ShaderVK::getVertexInputNames() const {
    return vertexInputNames;
}

const vk::PipelineShaderStageCreateInfo* ShaderVK::getShaderStageInfo() const {
    return shaderStageInfo;
}

const vk::PipelineVertexInputStateCreateInfo* ShaderVK::getVertexInputInfo() const {
    return &vertexInputInfo;
}

vk::PipelineLayout ShaderVK::getLayout() const {
    return layout;
}

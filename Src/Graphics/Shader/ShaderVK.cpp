#include "ShaderVK.h"

#include <PGE/Exception/Exception.h>

#include "../GraphicsVK.h"

#include <spirv_reflect.h>

using namespace PGE;

ShaderVK::ShaderVK(Graphics& gfx, const FilePath& path) : Shader(path), graphics((GraphicsVK&)gfx), resourceManager(gfx) {
    vk::Device device = graphics.getDevice();

    // Shader.
    std::vector<uint8_t> shaderBinary = (path + "shader.spv").readBytes();
    vkShader = resourceManager.addNewResource<VKShader>(device, shaderBinary);

    // Reflect.
    SpvReflectShaderModule reflection; SpvReflectResult res = spvReflectCreateShaderModule(shaderBinary.size(), shaderBinary.data(), &reflection);
    asrt(res == SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS, "Reflection failed " + String::hexFromInt((u32)res));

    // Vertex input.
    vertexInputAttributes.reserve(reflection.input_variable_count);
    int vertexStride = 0;
    std::vector<StructuredData::ElemLayout::Entry> entries; entries.reserve(reflection.input_variable_count);
    // TODO: Add ArrayProxies to PGE because they're BALLING.
    for (const SpvReflectInterfaceVariable& inputVariable : vk::ArrayProxy(reflection.input_variable_count, reflection.input_variables[0])) {
        // We don't want built in variables.
        if (inputVariable.built_in != -1) {
            continue;
        }
        String name = inputVariable.name;
        // Strip the "input.".
        name = name.substr(name.findFirst(".") + 1);
        vertexInputAttributes.push_back(vk::VertexInputAttributeDescription(inputVariable.location, 0, (vk::Format)inputVariable.format, vertexStride));
        // TODO: Better way to get vertex input size?
        int size = 4 * (inputVariable.numeric.vector.component_count > 0 ? inputVariable.numeric.vector.component_count : 1);
        vertexStride += size;
        entries.emplace_back(name, size);
    }
    vertexLayout = StructuredData::ElemLayout(entries);

    // Push constants.
    // Members are supposed to begin with the vertex constants and not meant to be mixed.
    std::vector<vk::PushConstantRange> ranges;
    if (reflection.push_constant_block_count != 0) {
        // TODO: Investigte duplicate constant block.
        //auto lol = reflection.push_constant_blocks[1];
        //asrt(reflection.push_constant_block_count <= 1, "Too many push constants (" + String::from(reflection.push_constant_block_count) + ") in shader " + path.str());

        SpvReflectBlockVariable pushConstant = reflection.push_constant_blocks[0];
        String blockName = pushConstant.name;
        asrt(blockName == "vulkanConstants", "Invalid push constant (\"" + blockName + "\")");
        
        constantData.resize(pushConstant.size);

        ranges.reserve(2);
        vertexConstantSize = pushConstant.padded_size;
        for (const SpvReflectBlockVariable& cMember : vk::ArrayProxy(pushConstant.member_count, pushConstant.members)) {
            String name = cMember.name;
            bool isVert = name.substr(0, 4) == "vert";
            if (!isVert && fragmentConstantMap.empty()) {
                vertexConstantSize = cMember.absolute_offset;
                fragmentConstantSize = constantData.size() - vertexConstantSize;
            }
            (isVert ? vertexConstantMap : fragmentConstantMap).emplace(name.substr(5),
                ConstantVK(*this, isVert ? vk::ShaderStageFlagBits::eVertex : vk::ShaderStageFlagBits::eFragment, constantData.data(),
                    cMember.absolute_offset, cMember.size));
        }
        if (!vertexConstantMap.empty()) { ranges.emplace_back(vk::ShaderStageFlagBits::eVertex, 0, vertexConstantSize); }
        if (!fragmentConstantMap.empty()) { ranges.emplace_back(vk::ShaderStageFlagBits::eFragment, vertexConstantSize, pushConstant.padded_size - vertexConstantSize); }
    }

    textureCount = 0;
    for (const SpvReflectDescriptorBinding& binding : vk::ArrayProxy(reflection.descriptor_binding_count, reflection.descriptor_bindings)) {
        if (binding.descriptor_type == SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
            textureCount = binding.count;
            break;
        }
    }

    layout = resourceManager.addNewResource<VKPipelineLayout>(device, ranges,
        textureCount > 0 ? std::optional(graphics.getDescriptorSetLayout(textureCount)) : std::nullopt);

    spvReflectDestroyShaderModule(&reflection);

    // Both constructor values need to remain in memory!
    vertexInputBinding = vk::VertexInputBindingDescription(0, vertexStride, vk::VertexInputRate::eVertex);
    vertexInputInfo = vk::PipelineVertexInputStateCreateInfo({ }, 1, &vertexInputBinding, (uint32_t)vertexInputAttributes.size(), vertexInputAttributes.data());

    vk::PipelineShaderStageCreateInfo vertexInfo({ }, vk::ShaderStageFlagBits::eVertex, vkShader, "VS");
    vk::PipelineShaderStageCreateInfo fragmentInfo({ }, vk::ShaderStageFlagBits::eFragment, vkShader, "PS");
    shaderStageInfo[0] = vertexInfo;
    shaderStageInfo[1] = fragmentInfo;

    graphics.addShader(*this);
    uploadPipelines();
}

ShaderVK::~ShaderVK() {
    if (textureCount > 0) {
        graphics.dropDescriptorSetLayout(textureCount);
    }
    graphics.removeShader(*this);
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

void ShaderVK::pushAllConstantsLazy() {
    if (lastFramePushed == graphics.getFrameCounter()) { return; }
    lastFramePushed = graphics.getFrameCounter();

    if (vertexConstantSize > 0) {
        graphics.getCurrentCommandBuffer().pushConstants(getLayout(), vk::ShaderStageFlagBits::eVertex, 0, vertexConstantSize, constantData.data());
    }
    if (fragmentConstantSize > 0) {
        graphics.getCurrentCommandBuffer().pushConstants(getLayout(), vk::ShaderStageFlagBits::eFragment, vertexConstantSize, fragmentConstantSize, constantData.data() + vertexConstantSize);
    }
}

ShaderVK::ConstantVK::ConstantVK(ShaderVK& shader, vk::ShaderStageFlags stage, byte* data, int offset, int size)
    : shader(shader), stage(stage), data(data + offset), offset(offset), size(size) { }

void ShaderVK::ConstantVK::setValue(const Matrix4x4f& value) {
    memcpy(data, &value, sizeof(value)); push();
}

void ShaderVK::ConstantVK::setValue(const Vector2f& value) {
    float arr[] = { value.x, value.y };
    memcpy(data, arr, sizeof(value)); push();
}

void ShaderVK::ConstantVK::setValue(const Vector3f& value) {
    float arr[] = { value.x, value.y, value.z };
    memcpy(data, arr, sizeof(value)); push();
}

void ShaderVK::ConstantVK::setValue(const Vector4f& value) {
    float arr[] = { value.x, value.y, value.z, value.w };
    memcpy(data, arr, sizeof(value)); push();
}

void ShaderVK::ConstantVK::setValue(const Color& value) {
    float arr[] = { value.red, value.green, value.blue, value.alpha };
    memcpy(data, arr, sizeof(value)); push();
}

void ShaderVK::ConstantVK::setValue(float value) {
    memcpy(data, &value, sizeof(value)); push();
}

void ShaderVK::ConstantVK::setValue(u32 value) {
    memcpy(data, &value, sizeof(value)); push();
}

void ShaderVK::ConstantVK::push() {
    shader.graphics.getCurrentCommandBuffer().pushConstants(shader.getLayout(), stage, offset, size, data);
}

vk::PipelineLayout ShaderVK::getLayout() const {
    return layout;
}

VKPipeline::View& ShaderVK::PipelinePair::getPipeline(PrimitiveType type) {
    return type == PrimitiveType::TRIANGLE ? triPipeline : linePipeline;
}

void ShaderVK::uploadPipeline(VKPipeline::View& pipeline, vk::RenderPass pass, PrimitiveType type) {
    resourceManager.trash(pipeline);
    pipeline = resourceManager.addNewResource<VKPipeline>(graphics.getDevice(),
        shaderStageInfo, vertexInputInfo, layout, graphics.getPipelineInfo(), pass, type);
}

void ShaderVK::uploadPipelines() {
    for (auto& [_, pair] : rtPipelines) {
        resourceManager.trash(pair.triPipeline);
        resourceManager.trash(pair.linePipeline);
    }
    rtPipelines.clear(); // TODO: IDFK.

    // TODO: Be lazy?
    vk::RenderPass pass = graphics.getBasicRenderPass();
    uploadPipeline(basicPipeline.triPipeline, pass, PrimitiveType::TRIANGLE);
    uploadPipeline(basicPipeline.linePipeline, pass, PrimitiveType::LINE);
}

vk::Pipeline ShaderVK::getPipeline(PrimitiveType type) {
    const RenderInfo* ri = graphics.getRenderInfo();
    if (ri == nullptr) {
        return basicPipeline.getPipeline(type);
    } else {
        VKPipeline::View& pipeline = rtPipelines[ri->pass].getPipeline(type); // TODO: Stupid map lookup!
        if (!pipeline.isHoldingResource()) {
            uploadPipeline(pipeline, ri->pass, type);
        }
        return pipeline;
    }
}

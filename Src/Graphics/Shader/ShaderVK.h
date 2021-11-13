#ifndef PGEINTERNAL_SHADER_VK_H_INCLUDED
#define PGEINTERNAL_SHADER_VK_H_INCLUDED

#include <PGE/Graphics/Shader.h>

#include <unordered_map>
#include <unordered_set>

#include <vulkan/vulkan.hpp>

#include <PGE/ResourceManagement/RawWrapper.h>
#include "../../ResourceManagement/ResourceManagerVK.h"
#include "../../ResourceManagement/VK.h"

namespace PGE {

class ShaderVK : public Shader {
    public:
        ShaderVK(Graphics& gfx, const FilePath& path);
        ~ShaderVK();

        Constant& getVertexShaderConstant(const String& name) override;
        Constant& getFragmentShaderConstant(const String& name) override;

        void pushAllConstantsLazy();

        vk::PipelineLayout getLayout() const;

        void uploadPipelines();
        vk::Pipeline getPipeline(Mesh::PrimitiveType type);

        const vk::WriteDescriptorSet getWriter() const;

        bool hasFuck = false;
        vk::DescriptorSet fuckYou();

    private:
        GraphicsVK& graphics;

        u64 lastFramePushed = std::numeric_limits<u64>::max();

        std::vector<byte> constantData;
        int vertexConstantSize = 0;
        int fragmentConstantSize = 0;

        int textureCount;

        VKShader::View vkShader;

        VKDescriptorSetLayout::View descriptorLayout;

        std::array<RawWrapper<VKMemoryBuffer>::View, 1> uniformBuffer;
        std::vector<vk::DescriptorSet> uniformSets;
        int fuckingTextureBinding;

        vk::DescriptorBufferInfo bufferInfo;
        vk::WriteDescriptorSet descriptorWrite;

        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageInfo;

        vk::VertexInputBindingDescription vertexInputBinding;
        std::vector<vk::VertexInputAttributeDescription> vertexInputAttributes;
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

        VKPipelineLayout::View layout;

        class ConstantVK : public Constant {
            public:
                ConstantVK(ShaderVK& shader, vk::ShaderStageFlags stage, byte* data, int offset, int size);

                void setValue(const Matrix4x4f& value) override;
                void setValue(const Vector2f& value) override;
                void setValue(const Vector3f& value) override;
                void setValue(const Vector4f& value) override;
                void setValue(const Color& value) override;
                void setValue(float value) override;
                void setValue(u32 value) override;

                virtual void push() = 0;

            protected:
                ShaderVK& shader;
                vk::ShaderStageFlags stage;
                byte* data;

                int offset;
                int size;
        };

        class UniformConstantVK : public ConstantVK {
            public:
                using ConstantVK::ConstantVK;
                void push() override;
        };

        class PushConstantVK : public ConstantVK {
            public:
                using ConstantVK::ConstantVK;
                void push() override;
        };
        std::unordered_map<String::Key, std::unique_ptr<ConstantVK>> vertexConstantMap;
        std::unordered_map<String::Key, std::unique_ptr<ConstantVK>> fragmentConstantMap;
            
        struct PipelinePair {
            VKPipeline::View triPipeline;
            VKPipeline::View linePipeline;
            VKPipeline::View& getPipeline(Mesh::PrimitiveType type);
        };
        std::unordered_map<vk::RenderPass, PipelinePair> rtPipelines;
        PipelinePair basicPipeline;

        ResourceManagerVK resourceManager;

        void uploadPipeline(VKPipeline::View& pipeline, vk::RenderPass pass, Mesh::PrimitiveType type);
};

}

#endif // PGEINTERNAL_SHADER_VK_H_INCLUDED

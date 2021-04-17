#ifndef PGEINTERNAL_SHADER_VK_H_INCLUDED
#define PGEINTERNAL_SHADER_VK_H_INCLUDED

#include <Shader/Shader.h>

#include <map>

#include <vulkan/vulkan.hpp>

#include "../ResourceManagement/ResourceManagerVK.h"
#include "../ResourceManagement/VK.h"

namespace PGE {

    class ShaderVK : public Shader {
        public:
            ShaderVK(Graphics* gfx, const FilePath& path);

            Constant* getVertexShaderConstant(String name) override;
            Constant* getFragmentShaderConstant(String name) override;

            int getVertexStride() const;
            std::vector<String> getVertexInputNames() const;

            vk::PipelineShaderStageCreateInfo* getShaderStageInfo();
            vk::PipelineVertexInputStateCreateInfo* getVertexInputInfo();
            const vk::PipelineLayout* getLayout();

        private:
            GraphicsVK* graphics;

            int vertexStride;
            std::vector<String> vertexInputNames;

            VKShader::Ref vkShader;

            vk::PipelineShaderStageCreateInfo shaderStageInfo[2];

            vk::VertexInputBindingDescription vertexInputBinding;
            std::vector<vk::VertexInputAttributeDescription> vertexInputAttributes;
            vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

            VKPipelineLayout::Ref layout;

            class ConstantVK : public Constant {
                public:
                    ConstantVK(Graphics* gfx, ShaderVK* she, vk::ShaderStageFlags stg, int off);

                    void setValue(Matrix4x4f value) override;
                    void setValue(Vector2f value) override;
                    void setValue(Vector3f value) override;
                    void setValue(Vector4f value) override;
                    void setValue(Color value) override;
                    void setValue(float value) override;
                    void setValue(int value) override;

                private:
                    Graphics* graphics;
                    ShaderVK* shader;
                    vk::ShaderStageFlags stage;
                    int offset;
            };
            std::map<long long, std::unique_ptr<ConstantVK>> vertexConstantMap;
            std::map<long long, std::unique_ptr<ConstantVK>> fragmentConstantMap;

            ResourceManagerVK resourceManager;
    };

}

#endif // PGEINTERNAL_SHADER_VK_H_INCLUDED

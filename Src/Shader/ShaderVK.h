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

            Constant* getVertexShaderConstant(const String& name) override;
            Constant* getFragmentShaderConstant(const String& name) override;

            int getVertexStride() const;
            const std::vector<String>& getVertexInputNames() const;

            const vk::PipelineShaderStageCreateInfo* getShaderStageInfo() const;
            const vk::PipelineVertexInputStateCreateInfo* getVertexInputInfo() const;
            vk::PipelineLayout getLayout() const;

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

                    void setValue(const Matrix4x4f& value) override;
                    void setValue(const Vector2f& value) override;
                    void setValue(const Vector3f& value) override;
                    void setValue(const Vector4f& value) override;
                    void setValue(const Color& value) override;
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

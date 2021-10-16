#ifndef PGEINTERNAL_SHADER_VK_H_INCLUDED
#define PGEINTERNAL_SHADER_VK_H_INCLUDED

#include <PGE/Graphics/Shader.h>

#include <unordered_map>
#include <unordered_set>

#include <vulkan/vulkan.hpp>

#include "../../ResourceManagement/ResourceManagerVK.h"
#include "../../ResourceManagement/VK.h"

namespace PGE {

    class ShaderVK : public Shader {
        public:
            ShaderVK(Graphics& gfx, const FilePath& path);
            ~ShaderVK();

            Constant& getVertexShaderConstant(const String& name) override;
            Constant& getFragmentShaderConstant(const String& name) override;

            void pushConstants();

            vk::PipelineLayout getLayout() const;

            void uploadPipelines();
            vk::Pipeline getPipeline(Mesh::PrimitiveType type) const;

        private:
            GraphicsVK& graphics;

            int textureCount;

            VKShader::View vkShader;

            std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageInfo;

            vk::VertexInputBindingDescription vertexInputBinding;
            std::vector<vk::VertexInputAttributeDescription> vertexInputAttributes;
            vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

            VKPipelineLayout::View layout;

            VKPipeline::View triPipeline;
            VKPipeline::View linePipeline;

            class ConstantVK : public Constant {
                public:
                    ConstantVK(ShaderVK* she, vk::ShaderStageFlags stg, int off);

                    void setValue(const Matrix4x4f& value) override;
                    void setValue(const Vector2f& value) override;
                    void setValue(const Vector3f& value) override;
                    void setValue(const Vector4f& value) override;
                    void setValue(const Color& value) override;
                    void setValue(float value) override;
                    void setValue(u32 value) override;

                    void push(GraphicsVK* gfx);

                private:
                    // TODO: Remove stinky, this is only temporary.
                    enum class Type {
                        MATRIX,
                        VECTOR2F,
                        VECTOR3F,
                        VECTOR4F,
                        COLOR,
                        FLOAT,
                        INT,
                    } valueType;
                    union {
                        Matrix4x4f matrixVal = Matrices::ZERO;
                        Vector2f vector2fVal;
                        Vector3f vector3fVal;
                        Vector4f vector4fVal;
                        Color colorVal;
                        float floatVal;
                        int intVal;
                    } val;

                    ShaderVK* shader;
                    vk::ShaderStageFlags stage;
                    int offset;
            };
            std::unordered_map<String::Key, ConstantVK> vertexConstantMap;
            std::unordered_map<String::Key, ConstantVK> fragmentConstantMap;
            std::unordered_set<ConstantVK*> updatedConstants;

            ResourceManagerVK resourceManager;

            template <VKPipeline::View ShaderVK::*PIPELINE, Mesh::PrimitiveType TYPE>
            void uploadPipeline();
    };

}

#endif // PGEINTERNAL_SHADER_VK_H_INCLUDED

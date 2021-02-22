#include <Shader/Shader.h>

#include <map>

#include <vulkan/vulkan.hpp>

#include "../Graphics/GraphicsVK.h"

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
            vk::PipelineLayout* getLayout();

        private:
            GraphicsVK* graphics;
            vk::Device device;

            int vertexStride;
            std::vector<String> vertexInputNames;

            vk::ShaderModule vkShader;

            vk::PipelineShaderStageCreateInfo shaderStageInfo[2];

            vk::VertexInputBindingDescription vertexInputBinding;
            std::vector<vk::VertexInputAttributeDescription> vertexInputAttributes;
            vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

            vk::PipelineLayout layout;

            class ConstantVK : public Constant {
                public:
                    ConstantVK(Graphics* gfx, vk::PipelineLayout lay, vk::ShaderStageFlags stg, int off);
                    ~ConstantVK() {};

                    void setValue(Matrix4x4f value) override;
                    void setValue(Vector2f value) override;
                    void setValue(Vector3f value) override;
                    void setValue(Vector4f value) override;
                    void setValue(Color value) override;
                    void setValue(float value) override;
                    void setValue(int value) override;

                private:
                    Graphics* graphics;
                    vk::PipelineLayout layout;
                    vk::ShaderStageFlags stage;
                    int offset;
            };
            std::map<long long, ConstantVK*> vertexConstantMap;
            std::map<long long, ConstantVK*> fragmentConstantMap;
    };

}

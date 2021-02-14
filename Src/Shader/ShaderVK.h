#include <Shader/Shader.h>

#include <vulkan/vulkan.hpp>

namespace PGE {

class ShaderVK : public Shader {
    public:
        ShaderVK(Graphics* gfx, const FilePath& path);
        ~ShaderVK();

        Constant* getVertexShaderConstant(String name) override;
        Constant* getFragmentShaderConstant(String name) override;

        vk::ShaderModule* getModules();
        vk::PipelineShaderStageCreateInfo* getShaderStageInfo();
        vk::PipelineVertexInputStateCreateInfo* getVertexInputInfo();
        vk::PipelineLayout* getLayout();

    private:
        Constant* tempConstant; // TODO: Remove with actual implementation.

        void cleanup() override;
        void throwException(String func, String details) override;

        vk::Device device;

        vk::ShaderModule vertexShader;
        vk::ShaderModule fragmentShader;
        vk::ShaderModule modules[2];

        vk::PipelineShaderStageCreateInfo shaderStageInfo[2];

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

        vk::PipelineLayout layout;

        class ConstantVK : public Constant {
            public:
                ConstantVK();
                ~ConstantVK(){};

                void setValue(Matrix4x4f value) override;
                void setValue(Vector2f value) override;
                void setValue(Vector3f value) override;
                void setValue(Vector4f value) override;
                void setValue(Color value) override;
                void setValue(float value) override;
                void setValue(int value) override;

            private:
                void throwException(String func, String details) override;
        };
};

}

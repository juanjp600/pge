#ifndef PGEINTERNAL_SHADER_VK_H_INCLUDED
#define PGEINTERNAL_SHADER_VK_H_INCLUDED

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

            int vertexStride;
            std::vector<String> vertexInputNames;

            SmartRef<vk::ShaderModule> vkShader;

            vk::PipelineShaderStageCreateInfo shaderStageInfo[2];

            vk::VertexInputBindingDescription vertexInputBinding;
            std::vector<vk::VertexInputAttributeDescription> vertexInputAttributes;
            vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

            SmartRef<vk::PipelineLayout> layout;

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
            SmartRef<std::map<long long, ConstantVK*>> vertexConstantMap;
            SmartRef<std::map<long long, ConstantVK*>> fragmentConstantMap;

            // Static class member because ConstantVK is private.
            static void clearMap(const std::map<long long, PGE::ShaderVK::ConstantVK*>& m);

            SmartOrderedDestructor destructor = 4;
    };

}

#endif // PGEINTERNAL_SHADER_VK_H_INCLUDED

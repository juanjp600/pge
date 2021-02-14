#ifndef PGEINTERNAL_MESH_VK_H_INCLUDED
#define PGEINTERNAL_MESH_VK_H_INCLUDED

#include <vulkan/vulkan.hpp>

#include <Mesh/Mesh.h>

namespace PGE {

class MeshVK : public Mesh {
    public:
        MeshVK(Graphics* gfx, Primitive::TYPE pt);
        ~MeshVK();

        void updateInternalData() override;
        void render() override;

    private:
        vk::Pipeline pipeline;

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

        void cleanup() override;
        void throwException(String func, String details) override;

        void uploadInternalData() override;
};

}

#endif // PGEINTERNAL_MESH_VK_H_INCLUDED

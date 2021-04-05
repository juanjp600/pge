#ifndef PGEINTERNAL_MESH_VK_H_INCLUDED
#define PGEINTERNAL_MESH_VK_H_INCLUDED

#include <Mesh/Mesh.h>

#include <vulkan/vulkan.hpp>

#include "../ResourceManagement/ResourceManagerVK.h"
#include "../ResourceManagement/VK.h"

namespace PGE {

class MeshVK : public Mesh {
    public:
        MeshVK(Graphics* gfx, Primitive::TYPE pt);

        void updateInternalData() override;
        void render() override;

    private:
        VKPipeline::Ref pipeline;

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

        // TODO: Combine buffers??
        VKBuffer::Ref dataBuffer;
        VKMemory::Ref dataMemory;

        ResourceManagerVK resourceManager;

        int totalVertexSize;
        int indicesCount;

        void uploadInternalData() override;
};

}

#endif // PGEINTERNAL_MESH_VK_H_INCLUDED

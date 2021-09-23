#ifndef PGEINTERNAL_MESH_VK_H_INCLUDED
#define PGEINTERNAL_MESH_VK_H_INCLUDED

#include <PGE/Graphics/Mesh.h>

#include <vulkan/vulkan.hpp>

#include "../../ResourceManagement/ResourceManagerVK.h"
#include "../../ResourceManagement/VK.h"

namespace PGE {

class MeshVK : public Mesh {
    public:
        MeshVK(Graphics& gfx);
        ~MeshVK();

        void uploadPipeline();

    private:
        GraphicsVK& graphics;

        VKPipeline::View pipeline;

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

        // TODO: Combine buffers??
        VKBuffer::View dataBuffer;
        VKMemory::View dataMemory;

        ResourceManagerVK resourceManager;

        int totalVertexSize;

        void uploadInternalData() override;
        void renderInternal() override;
};

}

#endif // PGEINTERNAL_MESH_VK_H_INCLUDED

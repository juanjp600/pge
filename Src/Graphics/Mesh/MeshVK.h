#ifndef PGEINTERNAL_MESH_VK_H_INCLUDED
#define PGEINTERNAL_MESH_VK_H_INCLUDED

#include <PGE/Graphics/Mesh.h>
#include <PGE/ResourceManagement/RawWrapper.h>

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
        class StagingCacheHandle {
            public:
                StagingCacheHandle(Graphics& gfx);
                ~StagingCacheHandle();

                void clear();
                void cache(int size);

            private:
                GraphicsVK& graphics;
                int heldSize = 0;
        } stagingCacheHandle;

        GraphicsVK& graphics;

        VKPipeline::View pipeline;

        UpdateStrategy oldStrat = strategy;

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

        RawWrapper<VKMemoryBuffer>::View data;
        int dataCapacity = 0;

        ResourceManagerVK resourceManager;

        int totalVertexSize;
        int bufferSize = 0;

        void uploadInternalData() override;
        void renderInternal() override;
};

}

#endif // PGEINTERNAL_MESH_VK_H_INCLUDED

#ifndef PGEINTERNAL_MESH_VK_H_INCLUDED
#define PGEINTERNAL_MESH_VK_H_INCLUDED

#include <vulkan/vulkan.hpp>

#include <Mesh/Mesh.h>

#include "../Misc/SmartPrimitive.h"

namespace PGE {

class MeshVK : public Mesh {
    public:
        MeshVK(Graphics* gfx, Primitive::TYPE pt);

        void updateInternalData() override;
        void render() override;

    private:
        SmartRef<vk::Pipeline> pipeline;

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

        // TODO: Combine buffers??
        SmartRef<vk::Buffer> dataBuffer;
        SmartRef<vk::DeviceMemory> dataMemory;

        SmartOrderedDestructor destructor = 3;

        int totalVertexSize;
        int indicesCount;

        void createBuffer(int size, vk::BufferUsageFlags bufferUsage, vk::MemoryPropertyFlags memProps, vk::Buffer& buffer, vk::DeviceMemory& memory);

        void uploadInternalData() override;
};

}

#endif // PGEINTERNAL_MESH_VK_H_INCLUDED

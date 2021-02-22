#ifndef PGEINTERNAL_MESH_VK_H_INCLUDED
#define PGEINTERNAL_MESH_VK_H_INCLUDED

#include <vulkan/vulkan.hpp>

#include <Mesh/Mesh.h>

namespace PGE {

class MeshVK : public Mesh {
    public:
        MeshVK(Graphics* gfx, Primitive::TYPE pt);

        void updateInternalData() override;
        void render() override;

    private:
        vk::Pipeline pipeline;

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

        // TODO: Combine buffers.
        vk::Buffer dataBuffer;
        vk::DeviceMemory dataMemory;

        int totalVertexSize;
        int indicesCount;

        void createBuffer(int size, vk::BufferUsageFlags bufferUsage, vk::MemoryPropertyFlags memProps, vk::Buffer& buffer, vk::DeviceMemory& memory);

        void uploadInternalData() override;
};

}

#endif // PGEINTERNAL_MESH_VK_H_INCLUDED

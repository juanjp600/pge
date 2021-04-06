#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_VK
#define PGEINTERNAL_RESOURCEMANAGEMENT_VK

#include <ResourceManagement/ResourceManager.h>

#include "../Graphics/GraphicsVK.h"

#include <vulkan/vulkan.hpp>

namespace PGE {

template <class T>
class VKResource : public Resource<T> {
	public:
		vk::Device device;

	public:
		VKResource(vk::Device dev) {
			device = dev;
		}

		VKResource(Graphics* gfx) {
			device = ((GraphicsVK*)gfx)->getDevice();
		}
};

template <class T>
class VKDestroyResource : public VKResource<T> {
	public:
		using VKResource<T>::VKResource;

		~VKDestroyResource() {
			this->device.destroy(this->resource);
		}
};

template <class T>
class VKFreeResource : public VKResource<T> {
	public:
		using VKResource<T>::VKResource;

		~VKFreeResource() {
			this->device.free(this->resource);
		}
};

class VKBuffer : public VKDestroyResource<vk::Buffer> {
	public:
		VKBuffer(vk::Device dev, vk::DeviceSize size, vk::BufferUsageFlags bufferUsage) : VKDestroyResource(dev) {
			vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo({}, size, bufferUsage, vk::SharingMode::eExclusive);
			resource = device.createBuffer(bufferInfo);
		}

		__RES_MNGMT__REF_FACT_METH(VKBuffer)
};

class VKMemory : public VKFreeResource<vk::DeviceMemory> {
	public:
		VKMemory(Graphics* gfx, vk::Buffer buffer, vk::MemoryPropertyFlags memProps) : VKFreeResource(gfx) {
			vk::MemoryRequirements memReq = device.getBufferMemoryRequirements(buffer);
			int memType = ((GraphicsVK*)gfx)->findMemoryType(memReq.memoryTypeBits, memProps);

			vk::MemoryAllocateInfo memoryInfo = vk::MemoryAllocateInfo(memReq.size, memType);
			resource = device.allocateMemory(memoryInfo);

			device.bindBufferMemory(buffer, resource, 0);
		}

		__RES_MNGMT__REF_FACT_METH(VKMemory)
};

class VKPipeline : public VKDestroyResource<vk::Pipeline> {
	private:
		static constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyLines = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eLineList, false);
		static constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyTris = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, false);

	public:
		VKPipeline(Graphics* gfx, ShaderVK* shader, Primitive::TYPE primitive) : VKDestroyResource(gfx) {
			const vk::PipelineInputAssemblyStateCreateInfo* inputInfo;
			switch (primitive) {
				case Primitive::TYPE::LINE: {
					inputInfo = &inputAssemblyLines;
				} break;
				default:
				case Primitive::TYPE::TRIANGLE: {
					inputInfo = &inputAssemblyTris;
				} break;
			}
			resource = ((GraphicsVK*)gfx)->createPipeline(shader, inputInfo);
		}

		__RES_MNGMT__REF_FACT_METH(VKPipeline)
};

class VKPipelineLayout : public VKDestroyResource<vk::PipelineLayout> {
	public:
		VKPipelineLayout(vk::Device dev, const std::vector<vk::PushConstantRange>& ranges) : VKDestroyResource(dev) {
			vk::PipelineLayoutCreateInfo layoutInfo({}, 0, nullptr, (uint32_t)ranges.size(), ranges.data());
			resource = device.createPipelineLayout(layoutInfo);
		}

		__RES_MNGMT__REF_FACT_METH(VKPipelineLayout)
};

class VKShader : public VKDestroyResource<vk::ShaderModule> {
	public:
		VKShader(vk::Device dev, const std::vector<uint8_t>& shaderBinary) : VKDestroyResource(dev) {
			vk::ShaderModuleCreateInfo shaderCreateInfo = vk::ShaderModuleCreateInfo({}, shaderBinary.size(), (uint32_t*)shaderBinary.data());
			resource = device.createShaderModule(shaderCreateInfo);
		}

		__RES_MNGMT__REF_FACT_METH(VKShader)
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_VK

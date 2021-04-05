#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_VK
#define PGEINTERNAL_RESOURCEMANAGEMENT_VK

#include <ResourceManagement/ResourceManager.h>

#include "../Graphics/GraphicsVK.h"

#include <vulkan/vulkan.hpp>

namespace PGE {

class VKBuffer : public Resource<vk::Buffer> {
	private:
		vk::Device device;

	public:
		VKBuffer(vk::Device dev, vk::DeviceSize size, vk::BufferUsageFlags bufferUsage) {
			device = dev;

			vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo({}, size, bufferUsage, vk::SharingMode::eExclusive);
			resource = device.createBuffer(bufferInfo);
		}

		~VKBuffer() {
			device.destroy(resource);
		}

		__RES_MNGMT__REF_FACT_METH(VKBuffer, vk::Buffer)
};

class VKMemory : public Resource<vk::DeviceMemory> {
	private:
		vk::Device device;

	public:
		VKMemory(Graphics* gfx, vk::Buffer buffer, vk::MemoryPropertyFlags memProps) {
			GraphicsVK* graphics = (GraphicsVK*)gfx;
			device = graphics->getDevice();

			vk::MemoryRequirements memReq = device.getBufferMemoryRequirements(buffer);
			int memType = graphics->findMemoryType(memReq.memoryTypeBits, memProps);

			vk::MemoryAllocateInfo memoryInfo = vk::MemoryAllocateInfo(memReq.size, memType);
			resource = device.allocateMemory(memoryInfo);

			device.bindBufferMemory(buffer, resource, 0);
		}

		~VKMemory() {
			device.free(resource);
		}

		__RES_MNGMT__REF_FACT_METH(VKMemory, vk::DeviceMemory)
};

class VKPipeline : public Resource<vk::Pipeline> {
	private:
		static constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyLines = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eLineList, false);
		static constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyTris = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, false);

		vk::Device device;

	public:
		VKPipeline(Graphics* gfx, ShaderVK* shader, Primitive::TYPE primitive) {
			GraphicsVK* graphics = (GraphicsVK*)gfx;
			device = graphics->getDevice();
				
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
			resource = graphics->createPipeline(shader, inputInfo);
		}

		~VKPipeline() {
			device.destroy(resource);
		}

		__RES_MNGMT__REF_FACT_METH(VKPipeline, vk::Pipeline)
};

class VKPipelineLayout : public Resource<vk::PipelineLayout> {
	private:
			vk::Device device;

	public:
		VKPipelineLayout(vk::Device dev, const std::vector<vk::PushConstantRange>& ranges) {
			device = dev;

			vk::PipelineLayoutCreateInfo layoutInfo({}, 0, nullptr, ranges.size(), ranges.data());
			resource = device.createPipelineLayout(layoutInfo);
		}

		~VKPipelineLayout() {
			device.destroy(resource);
		}

		__RES_MNGMT__REF_FACT_METH(VKPipelineLayout, vk::PipelineLayout)
};

class VKShader : public Resource<vk::ShaderModule> {
	private:
		vk::Device device;

	public:
		VKShader(vk::Device dev, const std::vector<uint8_t>& shaderBinary) {
			device = dev;

			vk::ShaderModuleCreateInfo shaderCreateInfo = vk::ShaderModuleCreateInfo({}, shaderBinary.size(), (uint32_t*)shaderBinary.data());
			resource = device.createShaderModule(shaderCreateInfo);
		}

		~VKShader() {
			device.destroy(resource);
		}

		__RES_MNGMT__REF_FACT_METH(VKShader, vk::ShaderModule)
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_VK

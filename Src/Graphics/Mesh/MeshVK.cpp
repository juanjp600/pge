#include "MeshVK.h"

#include <PGE/Exception/Exception.h>
#include <PGE/Graphics/Graphics.h>

#include "../Material/MaterialVK.h"
#include "../Shader/ShaderVK.h"
#include "../GraphicsVK.h"

using namespace PGE;

MeshVK::MeshVK(Graphics& gfx) : resourceManager(gfx), graphics((GraphicsVK&)gfx) {
	graphics.addMesh(*this);
	// TODO test over frames in flight.
}

MeshVK::~MeshVK() {
	graphics.removeMesh(*this);
}

void MeshVK::renderInternal() {
	((ShaderVK&)material->getShader()).pushConstants();

	vk::CommandBuffer comBuffer = graphics.getCurrentCommandBuffer();
	// TODO: How the fuck does the buffer know how much vertex data there is???
	comBuffer.bindVertexBuffers(0, dataBuffer.get(), (vk::DeviceSize)0);
	comBuffer.bindIndexBuffer(dataBuffer, totalVertexSize, vk::IndexType::eUint16);
	comBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	if (material->getTextureCount() > 0) {
		comBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			((ShaderVK&)material->getShader()).getLayout(), 0,
			((MaterialVK*)material)->getDescriptorSet(), { });
	}
	comBuffer.drawIndexed((u32)indices.size(), 1, 0, 0, 0);
}

// TODO: Crash when no vertices.
void MeshVK::uploadInternalData() {
	vk::Device device = graphics.getDevice();
	vk::PhysicalDevice physicalDevice = graphics.getPhysicalDevice();
	ShaderVK& shader = (ShaderVK&)material->getShader();

	resourceManager.trash(dataMemory);
	resourceManager.trash(dataBuffer);

	u32 vertexCount = vertices.getElementCount();
	totalVertexSize = shader.getVertexStride() * vertexCount;
	int finalTotalSize = totalVertexSize + sizeof(u16) * (int)indices.size();

	VKBuffer stagingBuffer = VKBuffer(device, finalTotalSize, vk::BufferUsageFlagBits::eTransferSrc);
	VKMemory stagingMemory = VKMemory(device, physicalDevice, stagingBuffer.get(), vk::MemoryPropertyFlagBits::eHostVisible);

	std::vector<String> vertexInputNames = shader.getVertexInputNames();
	float* vertexCursor = (float*)device.mapMemory(stagingMemory, 0, VK_WHOLE_SIZE);
	memcpy(vertexCursor, vertices.getData(), vertices.getDataSize());

	u16* indexCursor = (u16*)(((byte*)vertexCursor) + totalVertexSize);
	for (u32 u : indices) {
		*indexCursor = (u16)u;
		indexCursor++;
	}
	device.flushMappedMemoryRanges(vk::MappedMemoryRange(stagingMemory, 0, VK_WHOLE_SIZE));
	device.unmapMemory(stagingMemory);

	dataBuffer = resourceManager.addNewResource<VKBuffer>(device, finalTotalSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer);
	dataMemory = resourceManager.addNewResource<VKMemory>(device, physicalDevice, dataBuffer.get(), vk::MemoryPropertyFlagBits::eDeviceLocal);
	graphics.transfer(stagingBuffer, dataBuffer, finalTotalSize);

	uploadPipeline();
}

void MeshVK::uploadPipeline() {
	resourceManager.trash(pipeline);
	ShaderVK& shader = ((ShaderVK&)material->getShader());
	pipeline = resourceManager.addNewResource<VKPipeline>(graphics.getDevice(), shader.getShaderStageInfo(), shader.getVertexInputInfo(), shader.getLayout(), graphics.getPipelineInfo(), graphics.getRenderPass(), primitiveType.value());
}

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
	comBuffer.bindVertexBuffers(0, data->getBuffer(), (vk::DeviceSize)0);
	comBuffer.bindIndexBuffer(data->getBuffer(), totalVertexSize, vk::IndexType::eUint16);
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

	resourceManager.trash(data);

	u32 vertexCount = vertices.getElementCount();
	totalVertexSize = shader.getVertexStride() * vertexCount;
	int finalTotalSize = totalVertexSize + sizeof(u16) * (int)indices.size();

	// TODO: Don't allocate per update.
	VKMemoryBuffer staging(device, physicalDevice, finalTotalSize, VKMemoryBuffer::Type::STAGING);

	std::vector<String> vertexInputNames = shader.getVertexInputNames();
	float* vertexCursor = (float*)staging.getData();
	memcpy(vertexCursor, vertices.getData(), vertices.getDataSize());

	u16* indexCursor = (u16*)(((byte*)vertexCursor) + totalVertexSize);
	for (u32 u : indices) {
		*indexCursor = (u16)u;
		indexCursor++;
	}
	staging.flush(VK_WHOLE_SIZE);

	data = resourceManager.addNewResource<RawWrapper<VKMemoryBuffer>>(device, physicalDevice, finalTotalSize, VKMemoryBuffer::Type::DEVICE);
	graphics.transfer(staging.getBuffer(), data->getBuffer(), finalTotalSize);

	uploadPipeline();
}

void MeshVK::uploadPipeline() {
	resourceManager.trash(pipeline);
	ShaderVK& shader = ((ShaderVK&)material->getShader());
	pipeline = resourceManager.addNewResource<VKPipeline>(graphics.getDevice(), shader.getShaderStageInfo(), shader.getVertexInputInfo(), shader.getLayout(), graphics.getPipelineInfo(), graphics.getRenderPass(), primitiveType.value());
}

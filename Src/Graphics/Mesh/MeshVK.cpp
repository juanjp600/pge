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
	if (!data.isHoldingResource()) { return; }
	if (!pipeline.isHoldingResource()) { uploadPipeline(); }

	((ShaderVK&)material->getShader()).pushConstants();

	vk::CommandBuffer comBuffer = graphics.getCurrentCommandBuffer();
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

void MeshVK::uploadInternalData() {
	vk::Device device = graphics.getDevice();
	vk::PhysicalDevice physicalDevice = graphics.getPhysicalDevice();
	ShaderVK& shader = (ShaderVK&)material->getShader();

	totalVertexSize = vertices.getDataSize();
	if (totalVertexSize == 0) { return; }
	int finalTotalSize = totalVertexSize + sizeof(u16) * (int)indices.size();

	bool newData = finalTotalSize > dataCapacity || finalTotalSize * 4 < dataCapacity;
	if (oldStrat != strategy) {
		// We need to change the memory type.
		if (strategy == UpdateStrategy::PER_FRAME || oldStrat == UpdateStrategy::PER_FRAME) {
			newData = true;
		}
		oldStrat = strategy;
	}

	if (newData) {
		resourceManager.trash(data);
		data = resourceManager.addNewResource<RawWrapper<VKMemoryBuffer>>(device, physicalDevice, finalTotalSize,
			strategy == UpdateStrategy::PER_FRAME ? VKMemoryBuffer::Type::STAGING_DEVICE : VKMemoryBuffer::Type::DEVICE);

		dataCapacity = finalTotalSize;
	}

	VKMemoryBuffer* target;
	std::unique_ptr<VKMemoryBuffer> potStaging;
	if (strategy == UpdateStrategy::PER_FRAME) {
		target = data;
	} else {
		potStaging = std::make_unique<VKMemoryBuffer>(device, physicalDevice, finalTotalSize, VKMemoryBuffer::Type::STAGING);
		target = potStaging.get();
	}

	float* vertexCursor = (float*)target->getData();
	memcpy(vertexCursor, vertices.getData(), totalVertexSize);

	u16* indexCursor = (u16*)(((byte*)vertexCursor) + totalVertexSize);
	for (u32 u : indices) {
		*indexCursor = (u16)u;
		indexCursor++;
	}
	target->flush(Math::roundUp((vk::DeviceSize)finalTotalSize, graphics.getAtomSize()));

	if (target != data) {
		graphics.transfer(target->getBuffer(), data->getBuffer(), finalTotalSize);
	}
}

// TODO: Move this to material.
void MeshVK::uploadPipeline() {
	resourceManager.trash(pipeline);
	ShaderVK& shader = ((ShaderVK&)material->getShader());
	pipeline = resourceManager.addNewResource<VKPipeline>(graphics.getDevice(), shader.getShaderStageInfo(), shader.getVertexInputInfo(), shader.getLayout(), graphics.getPipelineInfo(), graphics.getRenderPass(), primitiveType.value());
}

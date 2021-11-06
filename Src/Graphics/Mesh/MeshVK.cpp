#include "MeshVK.h"

#include <PGE/Exception/Exception.h>
#include <PGE/Graphics/Graphics.h>

#include "../Material/MaterialVK.h"
#include "../Shader/ShaderVK.h"
#include "../GraphicsVK.h"

using namespace PGE;

MeshVK::StagingCacheHandle::StagingCacheHandle(Graphics& gfx) : graphics((GraphicsVK&)gfx) { }

MeshVK::StagingCacheHandle::~StagingCacheHandle() { clear(); }

void MeshVK::StagingCacheHandle::clear() {
	if (heldSize != 0) {
		graphics.unregisterStagingBuffer(heldSize);
		heldSize = 0;
	}
}

void MeshVK::StagingCacheHandle::cache(int size) {
	clear();
	graphics.registerStagingBuffer(size);
	heldSize = size;
}

MeshVK::MeshVK(Graphics& gfx) : resourceManager(gfx), graphics((GraphicsVK&)gfx), stagingCacheHandle(gfx) {
	// TODO test over frames in flight.
}

void MeshVK::renderInternal() {
	if (!data.isHoldingResource()) { return; }

	ShaderVK& shader = (ShaderVK&)material->getShader();
	shader.pushAllConstantsLazy();

	vk::CommandBuffer comBuffer = graphics.getCurrentCommandBuffer();
	comBuffer.bindVertexBuffers(0, data->getBuffer(), (vk::DeviceSize)0);
	comBuffer.bindIndexBuffer(data->getBuffer(), totalVertexSize, vk::IndexType::eUint16);
	comBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, shader.getPipeline(primitiveType.value()));
	if (material->getTextureCount() > 0) {
		comBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			((ShaderVK&)material->getShader()).getLayout(), 0,
			((MaterialVK*)material)->getDescriptorSet(), { });
	}
	comBuffer.drawIndexed((u32)indices.size(), 1, 0, 0, 0);
}

// VK updating strategies:
// Static: Using temporary staging buffer.
// Dynamic: Caching staging buffer.
// Per-frame: Not using a staging buffer, sharing memory between GPU and CPU.

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

		// We should no longer cache the staging buffer.
		if (oldStrat == UpdateStrategy::DYNAMIC) {
			stagingCacheHandle.clear();
		}

		oldStrat = strategy;
	}

	if (newData) {
		resourceManager.trash(data);
		data = resourceManager.addNewResource<RawWrapper<VKMemoryBuffer>>(device, physicalDevice, graphics.getAtomSize(), finalTotalSize,
			strategy == UpdateStrategy::PER_FRAME ? VKMemoryBuffer::Type::STAGING_DEVICE : VKMemoryBuffer::Type::DEVICE);

		if (strategy == UpdateStrategy::DYNAMIC) {
			stagingCacheHandle.cache(finalTotalSize);
		}

		dataCapacity = finalTotalSize;
	}

	VKMemoryBuffer* target;
	if (strategy == UpdateStrategy::PER_FRAME) {
		target = data;
	} else {
		target = &graphics.getTempStagingBuffer(finalTotalSize);
	}

	float* vertexCursor = (float*)target->getData();
	memcpy(vertexCursor, vertices.getData(), totalVertexSize);

	u16* indexCursor = (u16*)(((byte*)vertexCursor) + totalVertexSize);
	for (u32 u : indices) {
		*indexCursor = (u16)u;
		indexCursor++;
	}
	target->flush(finalTotalSize);

	if (target != data) {
		graphics.transfer(target->getBuffer(), data->getBuffer(), finalTotalSize);
	}
}

#include "MeshVK.h"

#include <Exception/Exception.h>
#include <Graphics/Graphics.h>

#include "../Shader/ShaderVK.h"
#include "../Graphics/GraphicsVK.h"

using namespace PGE;

MeshVK::MeshVK(Graphics* gfx, Primitive::Type pt) : resourceManager(gfx, 3) {
	graphics = gfx;
	primitiveType = pt;

	// TODO test over frames in flight.
}

// TODO: Crash when no vertices.
void MeshVK::updateInternalData() {
	if (!mustUpdateInternalData) { return; }

	GraphicsVK* graphics = (GraphicsVK*)this->graphics;
	vk::Device device = graphics->getDevice();
	vk::PhysicalDevice physicalDevice = graphics->getPhysicalDevice();
	ShaderVK* shader = (ShaderVK*)material->getShader();

	resourceManager.deleteResourcefromReference(pipeline);
	resourceManager.deleteResourcefromReference(dataMemory);
	resourceManager.deleteResourcefromReference(dataBuffer);

	totalVertexSize = shader->getVertexStride() * vertexCount;
	indicesCount = primitiveCount * (primitiveType == Primitive::Type::TRIANGLE ? 3 : 2);
	int finalTotalSize = totalVertexSize + sizeof(uint16_t) * indicesCount;

	VKBuffer stagingBuffer = VKBuffer(device, finalTotalSize, vk::BufferUsageFlagBits::eTransferSrc);
	VKMemory stagingMemory = VKMemory(device, physicalDevice, stagingBuffer, vk::MemoryPropertyFlagBits::eHostVisible);

	std::vector<String> vertexInputNames = shader->getVertexInputNames();
	float* vertexCursor = (float*)device.mapMemory(stagingMemory, 0, finalTotalSize);
	for (int i = 0; i < vertexCount; i++) {
		for (int j = 0; j < (int)vertexInputNames.size(); j++) {
			const Vertex::Property& prop = vertices[i].getProperty(vertexInputNames[j]);
			switch (prop.type) {
				case Vertex::Property::Type::FLOAT: {
					*vertexCursor = prop.value.floatVal;
					vertexCursor++;
				} break;
				case Vertex::Property::Type::UINT: {
					*((uint32_t*)vertexCursor) = prop.value.uintVal;
					vertexCursor++;
				} break;
				case Vertex::Property::Type::VECTOR2F: {
					*vertexCursor = prop.value.vector2fVal.x;
					vertexCursor++;
					*vertexCursor = prop.value.vector2fVal.y;
					vertexCursor++;
				} break;
				case Vertex::Property::Type::VECTOR3F: {
					*vertexCursor = prop.value.vector3fVal.x;
					vertexCursor++;
					*vertexCursor = prop.value.vector3fVal.y;
					vertexCursor++;
					*vertexCursor = prop.value.vector3fVal.z;
					vertexCursor++;
				} break;
				case Vertex::Property::Type::VECTOR4F: {
					*vertexCursor = prop.value.vector4fVal.x;
					vertexCursor++;
					*vertexCursor = prop.value.vector4fVal.y;
					vertexCursor++;
					*vertexCursor = prop.value.vector4fVal.z;
					vertexCursor++;
					*vertexCursor = prop.value.vector4fVal.w;
					vertexCursor++;
				} break;
				case Vertex::Property::Type::COLOR: {
					*vertexCursor = prop.value.colorVal.red;
					vertexCursor++;
					*vertexCursor = prop.value.colorVal.green;
					vertexCursor++;
					*vertexCursor = prop.value.colorVal.blue;
					vertexCursor++;
					*vertexCursor = prop.value.colorVal.alpha;
					vertexCursor++;
				} break;
			}
		}
	}

	uint16_t* indexCursor = (uint16_t*)vertexCursor;
	for (int i = 0; i < primitiveCount; i++) {
		*indexCursor = (uint16_t)primitives[i].a;
		indexCursor++;
		*indexCursor = (uint16_t)primitives[i].b;
		indexCursor++;
		if (primitiveType == Primitive::Type::TRIANGLE) {
			*indexCursor = (uint16_t)primitives[i].c;
			indexCursor++;
		}
	}
	device.flushMappedMemoryRanges(vk::MappedMemoryRange(stagingMemory, 0, VK_WHOLE_SIZE));
	device.unmapMemory(stagingMemory);

	dataBuffer = resourceManager.addNewResource<VKBuffer>(device, finalTotalSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer);
	dataMemory = resourceManager.addNewResource<VKMemory>(device, physicalDevice, dataBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
	graphics->transfer(stagingBuffer, dataBuffer, finalTotalSize);

	pipeline = resourceManager.addNewResource<VKPipeline>(device, shader->getShaderStageInfo(), shader->getVertexInputInfo(), shader->getLayout(), graphics->getPipelineInfo(), graphics->getRenderPass(), primitiveType);

	mustUpdateInternalData = false;
}

void MeshVK::render() {
	if (mustUpdateInternalData) {
		updateInternalData();
	}

	((ShaderVK*)material->getShader())->pushConstants();

	vk::CommandBuffer comBuffer = ((GraphicsVK*)graphics)->getCurrentCommandBuffer();
	// TODO: How the fuck does the buffer know how much vertex data there is???
	comBuffer.bindVertexBuffers(0, dataBuffer.get(), (vk::DeviceSize)0);
	comBuffer.bindIndexBuffer(dataBuffer, totalVertexSize, vk::IndexType::eUint16);
	comBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	comBuffer.drawIndexed(indicesCount, 1, 0, 0, 0);
}

void MeshVK::uploadInternalData() {

}

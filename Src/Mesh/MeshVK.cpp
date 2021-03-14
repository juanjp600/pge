#include "MeshVK.h"

#include <Exception/Exception.h>
#include <Graphics/Graphics.h>

#include "../Shader/ShaderVK.h"
#include "../Graphics/GraphicsVK.h"

using namespace PGE;

MeshVK::MeshVK(Graphics* gfx, Primitive::TYPE pt) {
	graphics = gfx;
	primitiveType = pt;

	destructor.setPreop(new GraphicsVK::OpDeviceIdle(((GraphicsVK*)graphics)->getDevice()));

	// TODO test over frames in flight.
	pipeline = destructor.getReference(vk::Pipeline(nullptr), ((GraphicsVK*)gfx)->getDevice(),
		+[](const vk::Pipeline& p, vk::Device d) { if (p != VK_NULL_HANDLE) { d.destroy(p); } });
	dataBuffer = destructor.getReference(vk::Buffer(nullptr), ((GraphicsVK*)gfx)->getDevice(),
		+[](const vk::Buffer& b, vk::Device d) { if (b != VK_NULL_HANDLE) { d.destroy(b); } });
	dataMemory = destructor.getReference(vk::DeviceMemory(nullptr), ((GraphicsVK*)gfx)->getDevice(),
		+[](const vk::DeviceMemory& m, vk::Device d) { if (m != VK_NULL_HANDLE) { d.free(m); } });
}

// TODO: Crash when no vertices.
void MeshVK::updateInternalData() {
	if (!mustUpdateInternalData) { return; }

	GraphicsVK* graphics = (GraphicsVK*)this->graphics;
	vk::Device device = graphics->getDevice();
	ShaderVK* shader = (ShaderVK*)material->getShader();

	// TODO: Own function?
	if (pipeline() != VK_NULL_HANDLE) {
		device.destroyPipeline(pipeline());
		device.destroyBuffer(dataBuffer());
		device.freeMemory(dataMemory());
	}

	totalVertexSize = shader->getVertexStride() * vertexCount;
	indicesCount = primitiveCount * (primitiveType == Primitive::TYPE::TRIANGLE ? 3 : 2);
	int finalTotalSize = totalVertexSize + sizeof(uint16_t) * indicesCount;

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingMemory;
	createBuffer(finalTotalSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible, stagingBuffer, stagingMemory);

	std::vector<String> vertexInputNames = shader->getVertexInputNames();
	std::vector<int> hintIndices(vertexInputNames.size());
	float* vertexCursor = (float*)device.mapMemory(stagingMemory, 0, finalTotalSize);
	for (int i = 0; i < vertexCount; i++) {
		for (int j = 0; j < (int)vertexInputNames.size(); j++) {
			const Vertex::Property& prop = vertices[i].getProperty(vertexInputNames[j], hintIndices[j]);
			switch (prop.type) {
				case Vertex::PROPERTY_TYPE::FLOAT: {
					*vertexCursor = prop.value.floatVal;
					vertexCursor++;
				} break;
				case Vertex::PROPERTY_TYPE::UINT: {
					*((uint32_t*)vertexCursor) = prop.value.uintVal;
					vertexCursor++;
				} break;
				case Vertex::PROPERTY_TYPE::VECTOR2F: {
					*vertexCursor = prop.value.vector2fVal.x;
					vertexCursor++;
					*vertexCursor = prop.value.vector2fVal.y;
					vertexCursor++;
				} break;
				case Vertex::PROPERTY_TYPE::VECTOR3F: {
					*vertexCursor = prop.value.vector3fVal.x;
					vertexCursor++;
					*vertexCursor = prop.value.vector3fVal.y;
					vertexCursor++;
					*vertexCursor = prop.value.vector3fVal.z;
					vertexCursor++;
				} break;
				case Vertex::PROPERTY_TYPE::VECTOR4F: {
					*vertexCursor = prop.value.vector4fVal.x;
					vertexCursor++;
					*vertexCursor = prop.value.vector4fVal.y;
					vertexCursor++;
					*vertexCursor = prop.value.vector4fVal.z;
					vertexCursor++;
					*vertexCursor = prop.value.vector4fVal.w;
					vertexCursor++;
				} break;
				case Vertex::PROPERTY_TYPE::COLOR: {
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
		if (primitiveType == Primitive::TYPE::TRIANGLE) {
			*indexCursor = (uint16_t)primitives[i].c;
			indexCursor++;
		}
	}
	device.flushMappedMemoryRanges(vk::MappedMemoryRange(stagingMemory, 0, VK_WHOLE_SIZE));
	device.unmapMemory(stagingMemory);

	createBuffer(finalTotalSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, dataBuffer(), dataMemory());
	graphics->transfer(stagingBuffer, dataBuffer(), finalTotalSize);

	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingMemory);

	pipeline = graphics->createPipeline(shader, primitiveType);

	mustUpdateInternalData = false;
}

void MeshVK::createBuffer(int size, vk::BufferUsageFlags bufferUsage, vk::MemoryPropertyFlags memProps, vk::Buffer& buffer, vk::DeviceMemory& memory) {
	GraphicsVK* graphics = (GraphicsVK*)this->graphics;
	const vk::Device& device = graphics->getDevice();

	vk::BufferCreateInfo indexBufferInfo = vk::BufferCreateInfo({}, size, bufferUsage, vk::SharingMode::eExclusive);
	buffer = device.createBuffer(indexBufferInfo);

	vk::MemoryRequirements memReq = device.getBufferMemoryRequirements(buffer);
	int memType = graphics->findMemoryType(memReq.memoryTypeBits, memProps);
	vk::MemoryAllocateInfo memoryInfo = vk::MemoryAllocateInfo(memReq.size, memType);

	memory = device.allocateMemory(memoryInfo);
	device.bindBufferMemory(buffer, memory, 0);
}

void MeshVK::render() {
	if (mustUpdateInternalData) {
		updateInternalData();
	}

	vk::CommandBuffer comBuffer = ((GraphicsVK*)graphics)->getCurrentCommandBuffer();
	// TODO: How the fuck does the buffer know how much vertex data there is???
	comBuffer.bindVertexBuffers(0, dataBuffer(), (vk::DeviceSize)0);
	comBuffer.bindIndexBuffer(dataBuffer(), totalVertexSize, vk::IndexType::eUint16);
	comBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline());
	comBuffer.drawIndexed(indicesCount, 1, 0, 0, 0);
}

void MeshVK::uploadInternalData() {

}

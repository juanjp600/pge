#include "MeshVK.h"

#include <Exception/Exception.h>
#include <Graphics/Graphics.h>

#include "../Shader/ShaderVK.h"
#include "../Window/WindowVK.h"
#include "../Graphics/GraphicsVK.h"

using namespace PGE;

MeshVK::MeshVK(Graphics* gfx, Primitive::TYPE pt) {
	graphics = gfx;

	primitiveType = pt;

	pipeline = vk::Pipeline(nullptr);
	vertexBuffer = vk::Buffer(nullptr);

	vk::PrimitiveTopology vkPrim;
	switch (pt) {
		case Primitive::TYPE::TRIANGLE: {
			vkPrim = vk::PrimitiveTopology::eTriangleList;
			break;
		}
		case Primitive::TYPE::LINE: {
			vkPrim = vk::PrimitiveTopology::eLineList;
			break;
		}
	}
	
	inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo({}, vkPrim, false);
}

MeshVK::~MeshVK() {
	cleanup();
}

// TODO: Crash when no vertices.
void MeshVK::updateInternalData() {
	if (!mustUpdateInternalData) { return; }

	WindowVK* window = (WindowVK*)graphics->getWindow();
	vk::Device device = window->getDevice();

	if (pipeline != VK_NULL_HANDLE) {
		device.destroyPipeline(pipeline);
		device.destroyBuffer(vertexBuffer);
		device.freeMemory(vertexMemory);
		device.destroyBuffer(indexBuffer);
		device.freeMemory(indexMemory);
	}

	int stride = sizeof(float) * 5;
	int totalSize = stride * vertexCount;

	vk::BufferCreateInfo vertexBufferInfo = vk::BufferCreateInfo({}, totalSize, vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive);
	vertexBuffer = device.createBuffer(vertexBufferInfo);
	vertexMemory = populateBuffer(vertexBuffer);
	device.bindBufferMemory(vertexBuffer, vertexMemory, 0);
	int indexInt = 0;
	int colorIndexInt = 1;
	float* vertexData = (float*)device.mapMemory(vertexMemory, 0, totalSize);
	for (int i = 0; i < vertexCount; i++) {
		Vertex::Property lol = vertices[i].getProperty("position", indexInt);
		*vertexData = lol.value.vector2fVal.x;
		vertexData++;
		*vertexData = lol.value.vector2fVal.y;
		vertexData++;
		Vertex::Property lol2 = vertices[i].getProperty("color", colorIndexInt);
		*vertexData = lol2.value.vector3fVal.x;
		vertexData++;
		*vertexData = lol2.value.vector3fVal.y;
		vertexData++;
		*vertexData = lol2.value.vector3fVal.z;
		vertexData++;
	}
	device.flushMappedMemoryRanges(vk::MappedMemoryRange(vertexMemory, 0, VK_WHOLE_SIZE));
	device.unmapMemory(vertexMemory);

	indicesCount = primitiveCount * (primitiveType == Primitive::TYPE::TRIANGLE ? 3 : 2);
	int indexTotalSize = sizeof(uint16_t) * indicesCount;
	vk::BufferCreateInfo indexBufferInfo = vk::BufferCreateInfo({}, indexTotalSize, vk::BufferUsageFlagBits::eIndexBuffer, vk::SharingMode::eExclusive);
	indexBuffer = device.createBuffer(indexBufferInfo);
	indexMemory = populateBuffer(indexBuffer);
	device.bindBufferMemory(indexBuffer, indexMemory, 0);
	uint16_t* indexData = (uint16_t*)device.mapMemory(indexMemory, 0, indexTotalSize);
	for (int i = 0; i < primitiveCount; i++) {
		*indexData = primitives[i].a;
		indexData++;
		*indexData = primitives[i].b;
		indexData++;
		if (primitiveType == Primitive::TYPE::TRIANGLE) {
			*indexData = primitives[i].c;
			indexData++;
		}
	}
	device.flushMappedMemoryRanges(vk::MappedMemoryRange(indexMemory, 0, VK_WHOLE_SIZE));
	device.unmapMemory(indexMemory);

	ShaderVK* shader = (ShaderVK*)material->getShader();
	vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo({}, 2, shader->getShaderStageInfo(), shader->getVertexInputInfo(), &inputAssemblyInfo, nullptr, window->getViewportInfo(), window->getRasterizationInfo(), window->getMultisamplerInfo(), nullptr, window->getColorBlendInfo(), nullptr, *shader->getLayout(), *window->getRenderPass(), 0, {}, -1);
	pipeline = device.createGraphicsPipeline(nullptr, pipelineInfo).value;

	mustUpdateInternalData = false;
}

vk::DeviceMemory MeshVK::populateBuffer(vk::Buffer buffer) {
	WindowVK* window = (WindowVK*)graphics->getWindow();
	vk::Device device = window->getDevice();

	vk::MemoryRequirements memReq = device.getBufferMemoryRequirements(buffer);
	int memType = window->findMemoryType(memReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);
	vk::MemoryAllocateInfo memoryInfo = vk::MemoryAllocateInfo(memReq.size, memType);
	return device.allocateMemory(memoryInfo);
}

void MeshVK::render() {
	if (mustUpdateInternalData) {
		updateInternalData();
	}

	vk::CommandBuffer comBuffer = ((GraphicsVK*)graphics)->getCurrentCommandBuffer();
	comBuffer.bindVertexBuffers(0, vertexBuffer, (vk::DeviceSize)0);
	comBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);
	comBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	comBuffer.drawIndexed(indicesCount, 1, 0, 0, 0);
}

void MeshVK::cleanup() {
	vk::Device device = ((GraphicsVK*)graphics)->getDevice();
	// TODO test over frames in flight
	device.waitIdle();
	if (pipeline != VK_NULL_HANDLE) {
		device.destroyPipeline(pipeline);
		device.destroyBuffer(vertexBuffer);
		device.freeMemory(vertexMemory);
		device.destroyBuffer(indexBuffer);
		device.freeMemory(indexMemory);
	}
}

void MeshVK::throwException(String func, String details) {
	cleanup();
	throw Exception("MeshVK::" + func, details);
}

void MeshVK::uploadInternalData() {

}

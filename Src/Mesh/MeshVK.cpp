#include "MeshVK.h"

#include <Exception/Exception.h>
#include <Graphics/Graphics.h>

#include "../Shader/ShaderVK.h"
#include "../Window/WindowVK.h"
#include "../Graphics/GraphicsVK.h"

using namespace PGE;

MeshVK::MeshVK(Graphics* gfx, Primitive::TYPE pt) {
	graphics = gfx;

	pipeline = VK_NULL_HANDLE;

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

void MeshVK::updateInternalData() {
	if (!mustUpdateInternalData) { return; }

	WindowVK* window = (WindowVK*)graphics->getWindow();
	vk::Device device = window->getDevice();

	if (pipeline != VK_NULL_HANDLE) {
		device.destroyPipeline(pipeline);
	}

	ShaderVK* shader = (ShaderVK*)material->getShader();
	vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo({}, 2, shader->getShaderStageInfo(), shader->getVertexInputInfo(), &inputAssemblyInfo, nullptr, window->getViewportInfo(), window->getRasterizationInfo(), window->getMultisamplerInfo(), nullptr, window->getColorBlendInfo(), nullptr, *shader->getLayout(), *window->getRenderPass(), 0, {}, -1);
	pipeline = device.createGraphicsPipeline(nullptr, pipelineInfo).value;

	mustUpdateInternalData = false;
}

void MeshVK::render() {
	if (mustUpdateInternalData) {
		updateInternalData();
	}

	vk::CommandBuffer comBuffer = ((GraphicsVK*)graphics)->getCurrentCommandBuffer();
	comBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	comBuffer.draw(3, 1, 0, 0);
}

void MeshVK::cleanup() {
	((GraphicsVK*)graphics)->getDevice().destroyPipeline(pipeline);
}

void MeshVK::throwException(String func, String details) {
	cleanup();
	throw Exception("MeshVK::" + func, details);
}

void MeshVK::uploadInternalData() {

}

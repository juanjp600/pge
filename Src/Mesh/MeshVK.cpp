#include "MeshVK.h"

#include <Exception/Exception.h>
#include <Graphics/Graphics.h>

#include "../Shader/ShaderVK.h"
#include "../Window/WindowVK.h"
#include "../Graphics/GraphicsVK.h"

using namespace PGE;

MeshVK::MeshVK(Graphics* gfx, Primitive::TYPE pt) {
	graphics = gfx;

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

}

void MeshVK::updateInternalData() {
	if (!mustUpdateInternalData) { return; }

	ShaderVK* shader = (ShaderVK*)material->getShader();
	WindowVK* window = (WindowVK*)graphics->getWindow();
	vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo({}, 2, shader->getShaderStageInfo(), shader->getVertexInputInfo(), &inputAssemblyInfo, nullptr, window->getViewportInfo(), window->getRasterizationInfo(), window->getMultisamplerInfo(), nullptr, window->getColorBlendInfo(), nullptr, *shader->getLayout(), *window->getRenderPass(), 0, {}, -1);
	pipeline = window->getDevice().createGraphicsPipeline(nullptr, pipelineInfo).value;

	// TODO: Move.
	std::vector<vk::CommandBuffer> comBuffers = ((GraphicsVK*)graphics)->getCommandBuffers();
	for (int i = 0; i < comBuffers.size(); i++) {
		comBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
		comBuffers[i].draw(3, 1, 0, 0);
		comBuffers[i].endRenderPass();
		comBuffers[i].end();
	}

	mustUpdateInternalData = false;
}

void MeshVK::render() {
	if (mustUpdateInternalData) {
		updateInternalData();
	}
	((GraphicsVK*)graphics)->startRender();
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

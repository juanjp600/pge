#include "GraphicsVK.h"

#include <Exception/Exception.h>

#include "../Window/WindowVK.h"

using namespace PGE;

GraphicsVK::GraphicsVK(String name, int w, int h, bool fs) {
    window = new WindowVK(name, w, h, fs);
}

GraphicsVK::~GraphicsVK() {
    cleanup();
}

Graphics::Renderer GraphicsVK::getRenderer() {
    return Renderer::Vulkan;
}

void GraphicsVK::setDepthTest(bool enabled) {
    depthTestEnabled = enabled;
}

bool GraphicsVK::getDepthTest() const {
    return depthTestEnabled;
}

void GraphicsVK::clear(Color color) {

}

void GraphicsVK::setRenderTarget(Texture* renderTarget) {

}

void GraphicsVK::setRenderTargets(std::vector<Texture*> renderTargets) {

}

void GraphicsVK::resetRenderTarget() {

}

void GraphicsVK::setViewport(Rectanglei vp) {

}

void GraphicsVK::cleanup() {
    if (window != nullptr) { delete window; }
    window = nullptr;
}

void GraphicsVK::throwException(String func, String details) {
    cleanup();
    throw Exception("GraphicsVK::" + func, details);
}

vk::Device GraphicsVK::getDevice() const {
    return ((WindowVK*)window)->getDevice();
}

std::vector<vk::CommandBuffer> GraphicsVK::getCommandBuffers() {
    return ((WindowVK*)window)->getCommandBuffers();
}

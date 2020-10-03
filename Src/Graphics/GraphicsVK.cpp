#include "GraphicsVK.h"

#include <Exception/Exception.h>

#include "../Window/WindowVK.h"

using namespace PGE;

#ifdef VULKAN
Graphics* Graphics::create(String name, int w, int h, bool fs) {
    return new GraphicsVK(name, w, h, fs);
}
#endif

GraphicsVK::GraphicsVK(String name, int w, int h, bool fs) {
    window = new WindowVK(name, w, h, fs);
}

GraphicsVK::~GraphicsVK() {
    cleanup();
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

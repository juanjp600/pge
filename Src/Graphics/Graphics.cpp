#include <Graphics/Graphics.h>
#include "../Exception/Exception.h"

using namespace PGE;

Graphics::Graphics() {
    
}

Window* Graphics::getWindow() const {
    return window;
}

void Graphics::update() {
    window->update();
}

void Graphics::swap(bool vsync) {
    try {
        window->swap(vsync);
    }  catch (Exception e) {
        cleanup();
        throw e;
    } catch (std::exception e) {
        cleanup();
        throw e;
    }
}

void Graphics::setViewport(Rectanglei vp) {
    viewport = vp;
}

Rectanglei Graphics::getViewport() const {
    return viewport;
}

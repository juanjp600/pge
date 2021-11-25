#include <PGE/Graphics/Graphics.h>

#include <SDL.h>

#include "GraphicsInternal.h"
#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

Graphics::Graphics(const String& name, int w, int h, WindowMode wm) {
    caption = name;
    dimensions = Vector2i(w, h); aspectRatio = (float)w / h;
    windowMode = wm;

    eventSubscriber = resourceManager.addNewResource<WindowEventSubscriber>(*this);

    open = true;
    focused = true;
}

void Graphics::update() {
    SDL_Event event;
    while (((SysEventsInternal::SubscriberInternal*)eventSubscriber.get())->popEvent(event)) {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            open = false;
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            focused = true;
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            focused = false;
        }
    }
}

void Graphics::setScreenPosition(const Vector2i& pos) const {
    SDL_SetWindowPosition(((GraphicsInternal*)this)->getWindow(), pos.x, pos.y);
}

const Rectanglei& Graphics::getViewport() const {
    return viewport;
}

const Vector2i& Graphics::getDimensions() const {
    return dimensions;
}

float Graphics::getAspectRatio() const {
    return aspectRatio;
}

bool Graphics::isWindowOpen() const {
    return open;
}

bool Graphics::isWindowFocused() const {
    return focused;
}

void Graphics::setDepthTest(bool isEnabled) {
    depthTest = isEnabled;
}

bool Graphics::getDepthTest() const {
    return depthTest;
}

void Graphics::setVsync(bool isEnabled) {
    vsync = isEnabled;
}

bool Graphics::getVsync() const {
    return vsync;
}

void Graphics::setCulling(CullingMode mode) {
    cullingMode = mode;
}

CullingMode Graphics::getCulling() const {
    return cullingMode;
}

Graphics::WindowEventSubscriber::WindowEventSubscriber(const Graphics& gfx) {
    resource = new SysEventsInternal::SubscriberInternal(gfx, EventType::WINDOW);
    SysEventsInternal::subscribe(resource);
}

Graphics::WindowEventSubscriber::~WindowEventSubscriber() {
    SysEventsInternal::unsubscribe(resource);
}

const int Graphics::DEFAULT_SCREEN_POSITION = SDL_WINDOWPOS_UNDEFINED;

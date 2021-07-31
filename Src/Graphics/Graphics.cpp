#include <PGE/Graphics/Graphics.h>

#include <SDL.h>

#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

Graphics::Graphics(const String& name, int w, int h, bool fs) {
    caption = name;
    dimensions = Vector2i(w, h); aspectRatio = (float)w / h;
    fullscreen = fs;

    eventSubscriber = resourceManager.addNewResource<WindowEventSubscriber>(this);

    open = true;
    focused = true;
}

Graphics::~Graphics() { }

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

void Graphics::setCulling(Culling mode) {
    cullingMode = mode;
}

Graphics::Culling Graphics::getCulling() const {
    return cullingMode;
}

Graphics::WindowEventSubscriber::WindowEventSubscriber(Graphics* gfx) {
    resource = new SysEventsInternal::SubscriberInternal(gfx, SysEventsInternal::SubscriberInternal::EventType::WINDOW);
    SysEventsInternal::subscribe(resource);
}

Graphics::WindowEventSubscriber::~WindowEventSubscriber() {
    SysEventsInternal::unsubscribe(resource);
}

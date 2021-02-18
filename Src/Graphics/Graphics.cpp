#include <Graphics/Graphics.h>

#include <SDL.h>

#include <Exception/Exception.h>
#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

Graphics::Graphics(String name, int w, int h, bool fs) {
    caption = name;
    width = w; height = h; fullscreen = fs;

    eventSubscriber = std::shared_ptr<SysEventsInternal::SubscriberInternal>(new SysEventsInternal::SubscriberInternal(this, SysEventsInternal::SubscriberInternal::EventType::WINDOW),
        [](SysEventsInternal::SubscriberInternal* s) { SysEventsInternal::unsubscribe(s); });
    SysEventsInternal::subscribe(eventSubscriber.get());

    open = true;
    focused = true;
}

void Graphics::update() {
    SDL_Event event;
    auto test = [](int a) {};
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

Rectanglei Graphics::getViewport() const {
    return viewport;
}

int Graphics::getWidth() const {
    return width;
}

int Graphics::getHeight() const {
    return height;
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

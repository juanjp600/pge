#include <PGE/Graphics/Graphics.h>

#include <SDL.h>

#include <PGE/Types/Types.h>
#include <PGE/Exception/Exception.h>
#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

std::list<Graphics*> Graphics::activeGraphics;

const std::list<Graphics*>& Graphics::getActiveInstances() {
    return activeGraphics;
}

Graphics::Graphics(const String& name, int w, int h, bool fs, u32 windowFlags) : resourceManager(2) {
    caption = name;
    dimensions = Vector2i(w, h); aspectRatio = (float)w / h;
    fullscreen = fs;

    eventSubscriber = resourceManager.takeOwnership(new WindowEventSubscriber(this));

    sdlWindow = resourceManager.takeOwnership(new SDLWindow(name, w, h, windowFlags));

    open = true;
    focused = true;

    activeGraphics.push_back(this);
}

Graphics::~Graphics() {
    activeGraphics.erase(std::find(activeGraphics.begin(), activeGraphics.end(), this));
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

#define APPEND(name) '\n' + #name + ": " + String::fromInt(name)

String Graphics::getInfo() const {
    return caption + " (" + getRendererName() + ") "
        + dimensions.toString() + " / " + String::fromInt(viewport.width()) + 'x' + String::fromInt(viewport.height())
        + APPEND(open) + APPEND(focused) + APPEND(fullscreen) + APPEND(vsync) + APPEND(depthTest);
}

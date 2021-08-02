#include <PGE/Graphics/Graphics.h>

#include <SDL.h>

#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

std::list<Graphics*> Graphics::activeGraphics;

const std::list<Graphics*>& Graphics::getActiveInstances() {
    return activeGraphics;
}

Graphics::Graphics(const String& name, int w, int h, bool fs, u32 windowFlags) {
    caption = name;
    dimensions = Vector2i(w, h); aspectRatio = (float)w / h;
    fullscreen = fs;

    eventSubscriber = resourceManager.addNewResource<WindowEventSubscriber>(*this);

    sdlWindow = resourceManager.addNewResource<SDLWindow>(name, w, h, windowFlags);

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

SDL_Window* Graphics::getWindow() const noexcept {
    return sdlWindow;
}

Graphics::WindowEventSubscriber::WindowEventSubscriber(Graphics& gfx) {
    resource = new SysEventsInternal::SubscriberInternal(gfx, SysEventsInternal::SubscriberInternal::EventType::WINDOW);
    SysEventsInternal::subscribe(resource);
}

Graphics::WindowEventSubscriber::~WindowEventSubscriber() {
    SysEventsInternal::unsubscribe(resource);
}

Graphics::SDLWindow::SDLWindow(const String& title, int width, int height, u32 flags) {
    resource = SDL_CreateWindow(title.cstr(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    PGE_ASSERT(resource != nullptr, "Failed to create SDL window (SDLERROR: " + String(SDL_GetError()) + ")");
}

Graphics::SDLWindow::~SDLWindow() {
    SDL_DestroyWindow(resource);
}

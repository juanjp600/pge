#include "Misc.h"

#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

WindowEventSubscriber::WindowEventSubscriber(Graphics* gfx) {
    resource = new SysEventsInternal::SubscriberInternal(gfx, SysEventsInternal::SubscriberInternal::EventType::WINDOW);
    SysEventsInternal::subscribe(resource);
}

WindowEventSubscriber::~WindowEventSubscriber() {
    SysEventsInternal::unsubscribe(resource);
}

SDLWindow::SDLWindow(const String& title, int width, int height, uint32_t flags) {
    resource = SDL_CreateWindow(title.cstr(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    if (resource == nullptr) {
        throw Exception("SDLWindow", "Failed to create SDL window: " + String(SDL_GetError()));
    }
}

SDLWindow::~SDLWindow() {
    SDL_DestroyWindow(resource);
}

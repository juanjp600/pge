#include "Misc.h"

#include <PGE/Types/Types.h>
#include <PGE/Exception/Exception.h>

#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

WindowEventSubscriber::WindowEventSubscriber(Graphics* gfx) {
    resource = new SysEventsInternal::SubscriberInternal(gfx, SysEventsInternal::SubscriberInternal::EventType::WINDOW);
    SysEventsInternal::subscribe(resource);
}

WindowEventSubscriber::~WindowEventSubscriber() {
    SysEventsInternal::unsubscribe(resource);
}

SDLWindow::SDLWindow(const String& title, int width, int height, u32 flags) {
    resource = SDL_CreateWindow(title.cstr(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    PGE_ASSERT(resource != nullptr,  "Failed to create SDL window (SDLERROR: " + String(SDL_GetError()) + ")");
}

SDLWindow::~SDLWindow() {
    SDL_DestroyWindow(resource);
}

#include <PGE/SysEvents/SysEvents.h>
#include "SysEventsInternal.h"
#include "../Graphics/GraphicsInternal.h"

using namespace PGE;

std::unordered_set<SysEvents::Subscriber*> SysEventsInternal::subscribers;

void SysEventsInternal::subscribe(SysEvents::Subscriber* subscriber) {
    subscribers.emplace(subscriber);
}

void SysEventsInternal::unsubscribe(SysEvents::Subscriber* subscriber) {
    std::unordered_set<Subscriber*>::iterator it = subscribers.find(subscriber);
    if (it != subscribers.end()) {
        subscribers.erase(it);
    }
}

void SysEvents::update() {
    SysEventsInternal::update();
}

void SysEventsInternal::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        for (SysEvents::Subscriber* sub : subscribers) {
            SubscriberInternal* subscriber = (SubscriberInternal*)sub;
            SDL_Window* sdlWindow = ((GraphicsInternal&)subscriber->getGraphics()).getWindow();
            bool takeEvent = false;
            if (subscriber->getEventType()==EventType::WINDOW) {
                if (event.type == SDL_WINDOWEVENT) {
                    takeEvent = event.window.windowID == SDL_GetWindowID(sdlWindow);
                }
            } else if (subscriber->getEventType()==EventType::KEYBOARD) {
                if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                    takeEvent = SDL_GetKeyboardFocus()==sdlWindow;
                }
            } else if (subscriber->getEventType()==EventType::MOUSE) {
                if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP ||
                    event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEWHEEL) {
                    takeEvent = SDL_GetMouseFocus()==sdlWindow;
                }
            } else if (subscriber->getEventType()==EventType::CONTROLLER) {
                if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP ||
                    event.type == SDL_CONTROLLERAXISMOTION ||
                    event.type == SDL_CONTROLLERDEVICEADDED || event.type == SDL_CONTROLLERDEVICEREMOVED ||
                    event.type == SDL_CONTROLLERDEVICEREMAPPED) {
                    takeEvent = true;
                }
            } else if (subscriber->getEventType()==EventType::TEXTINPUT) {
                if (event.type == SDL_TEXTINPUT) {
                    takeEvent = SDL_GetKeyboardFocus()==sdlWindow;
                }
            }
            if (takeEvent) {
                subscriber->pushEvent(event);
            }
        }
    }
}

SysEventsInternal::SubscriberInternal::SubscriberInternal(const Graphics& gfx, EventType et)
    : graphics(gfx) {
    eventType = et;
    receivedEvent = false;
    events.clear();
}

const Graphics& SysEventsInternal::SubscriberInternal::getGraphics() const {
    return graphics;
}

EventType SysEventsInternal::SubscriberInternal::getEventType() const {
    return eventType;
}

bool SysEventsInternal::SubscriberInternal::popEvent(SDL_Event& e) {
    if (events.size()==0) { return false; }
    e = events[0]; events.erase(events.begin());
    return true;
}

void SysEventsInternal::SubscriberInternal::pushEvent(SDL_Event e) {
    events.emplace_back(e);
}

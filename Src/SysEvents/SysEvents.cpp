#include <SysEvents/SysEvents.h>
#include "SysEventsInternal.h"
#include "../Window/WindowInternal.h"

using namespace PGE;

std::set<SysEvents::Subscriber*> SysEventsInternal::subscribers;

void SysEventsInternal::subscribe(SysEvents::Subscriber* subscriber) {
    subscribers.emplace(subscriber);
}

void SysEventsInternal::unsubscribe(SysEvents::Subscriber* subscriber) {
    std::set<Subscriber*>::iterator it = subscribers.find(subscriber);
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
        for (std::set<Subscriber*>::iterator it=subscribers.begin();it!=subscribers.end();it++) {
            SubscriberInternal* subscriber = (SubscriberInternal*)(*it);
            SDL_Window* sdlWindow = ((WindowInternal*)subscriber->getWindow())->getSdlWindow();
            bool takeEvent = false;
            if (subscriber->getEventType()==SubscriberInternal::EventType::WINDOW) {
                if (event.type == SDL_WINDOWEVENT) {
                    takeEvent = event.window.windowID == SDL_GetWindowID(sdlWindow);
                }
            } else if (subscriber->getEventType()==SubscriberInternal::EventType::KEYBOARD) {
                if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                    takeEvent = SDL_GetKeyboardFocus()==sdlWindow;
                }
            } else if (subscriber->getEventType()==SubscriberInternal::EventType::MOUSE) {
                if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP ||
                    event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEWHEEL) {
                    takeEvent = SDL_GetMouseFocus()==sdlWindow;
                }
            }
            //TODO: capture gamepad events
            if (takeEvent) {
                subscriber->pushEvent(event);
            }
        }
    }
}

SysEventsInternal::SubscriberInternal::SubscriberInternal() {}

SysEventsInternal::SubscriberInternal::SubscriberInternal(Window* w,EventType et) {
    window = w; eventType = et;
    events.clear();
}

Window* SysEventsInternal::SubscriberInternal::getWindow() const {
    return window;
}

SysEventsInternal::SubscriberInternal::EventType SysEventsInternal::SubscriberInternal::getEventType() const {
    return eventType;
}

bool SysEventsInternal::SubscriberInternal::popEvent(SDL_Event& e) {
    if (events.size()==0) { return false; }
    e = events[0]; events.erase(events.begin());
    return true;
}

void SysEventsInternal::SubscriberInternal::pushEvent(SDL_Event e) {
    events.push_back(e);
}

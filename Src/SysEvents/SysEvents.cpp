#include <SysEvents/SysEvents.h>

using namespace PGE;

std::set<SysEvents::Subscriber*> SysEvents::subscribers;

void SysEvents::subscribe(SysEvents::Subscriber& subscriber) {
    subscribers.emplace(&subscriber);
}

void SysEvents::unsubscribe(SysEvents::Subscriber& subscriber) {
    std::set<Subscriber*>::iterator it = subscribers.find(&subscriber);
    if (it != subscribers.end()) {
        subscribers.erase(it);
    }
}

void SysEvents::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        for (std::set<Subscriber*>::iterator it=subscribers.begin();it!=subscribers.end();it++) {
            Subscriber* subscriber = *it;
            bool takeEvent = false;
            if (subscriber->getEventType()==Subscriber::EventType::WINDOW) {
                if (event.type == SDL_WINDOWEVENT) {
                    takeEvent = event.window.windowID == SDL_GetWindowID(subscriber->getWindow());
                }
            } else if (subscriber->getEventType()==Subscriber::EventType::KEYBOARD) {
                if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                    takeEvent = SDL_GetKeyboardFocus()==subscriber->getWindow();
                }
            } else if (subscriber->getEventType()==Subscriber::EventType::MOUSE) {
                if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP ||
                    event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEWHEEL) {
                    takeEvent = SDL_GetMouseFocus()==subscriber->getWindow();
                }
            }
            //TODO: capture gamepad events
            if (takeEvent) {
                subscriber->pushEvent(event);
            }
        }
    }
}

SysEvents::Subscriber::Subscriber() {}

SysEvents::Subscriber::Subscriber(SDL_Window* w,EventType et) {
    window = w; eventType = et;
    events.clear();
}

SDL_Window* SysEvents::Subscriber::getWindow() const {
    return window;
}

SysEvents::Subscriber::EventType SysEvents::Subscriber::getEventType() const {
    return eventType;
}

bool SysEvents::Subscriber::popEvent(SDL_Event& e) {
    if (events.size()==0) { return false; }
    e = events[0]; events.erase(events.begin());
    return true;
}

void SysEvents::Subscriber::pushEvent(SDL_Event e) {
    events.push_back(e);
}

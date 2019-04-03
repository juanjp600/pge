#ifndef PGEINTERNAL_SYSEVENTSINTERNAL_H_INCLUDED
#define PGEINTERNAL_SYSEVENTSINTERNAL_H_INCLUDED

#include <SDL.h>
#include <set>
#include <vector>

#include <SysEvents/SysEvents.h>

namespace PGE {

class Window;
class SysEventsInternal : public SysEvents {
    public:
        class SubscriberInternal;
    private:
        static std::set<Subscriber*> subscribers;
        SysEventsInternal(){};
    public:
        static void subscribe(Subscriber* sub);
        static void unsubscribe(Subscriber* sub);
        static void update();

        class SubscriberInternal : public Subscriber {
            public:
                enum class EventType {
                    WINDOW,
                    KEYBOARD,
                    MOUSE,
                    GAMEPAD
                };
            private:
                Window* window;
                EventType eventType;
                std::vector<SDL_Event> events;
                bool receivedEvent;
            public:
                SubscriberInternal();
                SubscriberInternal(Window* w,EventType et);

                Window* getWindow() const;
                EventType getEventType() const;
                void pushEvent(SDL_Event e);
                bool popEvent(SDL_Event& e);
        };
};

}

#endif // SYSEVENTS_H_INCLUDED

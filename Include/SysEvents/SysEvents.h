#ifndef PGE_SYSEVENTS_H_INCLUDED
#define PGE_SYSEVENTS_H_INCLUDED

#include <SDL.h>
#include <set>
#include <vector>

namespace PGE {

class SysEvents {
    public:
        class Subscriber;
    private:
        static std::set<Subscriber*> subscribers;
        SysEvents(){};
    public:
        static void subscribe(Subscriber& sub);
        static void unsubscribe(Subscriber& sub);
        static void update();

        class Subscriber {
            public:
                enum class EventType {
                    WINDOW,
                    KEYBOARD,
                    MOUSE,
                    GAMEPAD
                };
            private:
                SDL_Window* window;
                EventType eventType;
                std::vector<SDL_Event> events;
                bool receivedEvent;
            public:
                Subscriber();
                Subscriber(SDL_Window* w,EventType et);

                SDL_Window* getWindow() const;
                EventType getEventType() const;
                void pushEvent(SDL_Event e);
                bool popEvent(SDL_Event& e);
        };
};

}

#endif // SYSEVENTS_H_INCLUDED

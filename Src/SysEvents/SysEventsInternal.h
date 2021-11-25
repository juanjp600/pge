#ifndef PGEINTERNAL_SYSEVENTSINTERNAL_H_INCLUDED
#define PGEINTERNAL_SYSEVENTSINTERNAL_H_INCLUDED

#include <unordered_set>
#include <vector>

#include <SDL.h>

#include <PGE/SysEvents/SysEvents.h>

namespace PGE {

class Graphics;

enum class EventType {
    WINDOW,
    KEYBOARD,
    MOUSE,
    CONTROLLER,
    TEXTINPUT,
};

class SysEventsInternal : public SysEvents {
    private:
        static std::unordered_set<Subscriber*> subscribers;
        SysEventsInternal() = default;

    public:
        static void subscribe(Subscriber* sub);
        static void unsubscribe(Subscriber* sub);
        static void update();

        class SubscriberInternal : public Subscriber {
            private:
                const Graphics& graphics;
                EventType eventType;
                std::vector<SDL_Event> events;
                bool receivedEvent;

            public:
                SubscriberInternal() = default;
                SubscriberInternal(const Graphics& gfx, EventType et);

                const Graphics& getGraphics() const;
                EventType getEventType() const;
                void pushEvent(SDL_Event e);
                bool popEvent(SDL_Event& e);
        };
};

}

#endif // PGEINTERNAL_SYSEVENTSINTERNAL_H_INCLUDED

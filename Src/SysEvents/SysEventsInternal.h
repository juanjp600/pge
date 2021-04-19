#ifndef PGEINTERNAL_SYSEVENTSINTERNAL_H_INCLUDED
#define PGEINTERNAL_SYSEVENTSINTERNAL_H_INCLUDED

#include <unordered_set>
#include <vector>

#include <SDL.h>

#include <SysEvents/SysEvents.h>

namespace PGE {

class Graphics;

class SysEventsInternal : public SysEvents {
    public:
        class SubscriberInternal;

    private:
        static std::unordered_set<Subscriber*> subscribers;
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
                    CONTROLLER,
                    TEXTINPUT
                };

            private:
                Graphics* graphics;
                EventType eventType;
                std::vector<SDL_Event> events;
                bool receivedEvent;

            public:
                SubscriberInternal();
                SubscriberInternal(Graphics* gfx,EventType et);

                Graphics* getGraphics() const;
                EventType getEventType() const;
                void pushEvent(SDL_Event e);
                bool popEvent(SDL_Event& e);
        };
};

}

#endif // PGEINTERNAL_SYSEVENTSINTERNAL_H_INCLUDED

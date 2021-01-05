#ifndef PGE_WINDOW_H_INCLUDED
#define PGE_WINDOW_H_INCLUDED

#include <Misc/String.h>
#include <SysEvents/SysEvents.h>

namespace PGE {

class Window {
    public:
        virtual void update() =0;
        virtual void swap(bool vsyncEnabled=true) = 0;
        virtual bool isOpen() const;

        int getWidth() const;
        int getHeight() const;

        virtual bool isFocused() const;

        virtual ~Window(){};

    protected:
        Window(){};
        virtual void cleanup() = 0;
        virtual void throwException(String func,String details) = 0;

        String caption;
        int width; int height; bool fullscreen;

        SysEvents::Subscriber* eventSubscriber;

        bool open;
        bool focused;
};

}

#endif // PGE_WINDOW_H_INCLUDED

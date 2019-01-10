#ifndef PGE_WINDOW_H_INCLUDED
#define PGE_WINDOW_H_INCLUDED

#include <SDL.h>
#include <Misc/String.h>
#include <SysEvents/SysEvents.h>

namespace PGE {

class Window {
    public:
        virtual void update() =0;
        virtual void swap(bool vsyncEnabled=true) =0;
        virtual bool isOpen() const;

        int getWidth() const;
        int getHeight() const;

        SDL_Window* getSdlWindow() const; //TODO: move to derived classes

        ~Window(){};
    protected:
        Window(){};

        String caption;
        int width; int height; bool fullscreen;

        SDL_Window* sdlWindow;
        SysEvents::Subscriber eventSubscriber;

        bool open;
        bool vsync;
};

}

#endif

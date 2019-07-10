#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <SDL.h>
#include <Window/Window.h>

#ifdef __APPLE__
#ifdef __OBJC__
#import <AppKit/AppKit.h>
#endif
#endif

namespace PGE {

class WindowInternal : public Window {
    public:
        SDL_Window* getSdlWindow() const;
#ifdef __APPLE__
#ifdef __OBJC__
        NSWindow* getCocoaWindow() const;
#endif
#endif
    protected:
        SDL_Window* sdlWindow;
};

}

#endif

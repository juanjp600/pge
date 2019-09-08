#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <SDL.h>
#include <Window/Window.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#endif

namespace PGE {

class WindowInternal : public Window {
    public:
        SDL_Window* getSdlWindow() const;
#if defined(__APPLE__) && defined(__OBJC__)
        NSWindow* getCocoaWindow() const;
#endif
    protected:
        SDL_Window* sdlWindow;
};

}

#endif

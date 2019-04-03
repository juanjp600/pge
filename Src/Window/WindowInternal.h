#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <SDL.h>
#include <Window/Window.h>

namespace PGE {

class WindowInternal : public Window {
    public:
        SDL_Window* getSdlWindow() const;
    protected:
        SDL_Window* sdlWindow;
};

}

#endif

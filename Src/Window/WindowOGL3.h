#ifndef PGEINTERNAL_WINDOWOGL3_H_INCLUDED
#define PGEINTERNAL_WINDOWOGL3_H_INCLUDED

#ifdef LINUX
#define SDL_VIDEO_DRIVER_X11
#endif
#include <SDL.h>

#include <Window/Window.h>
#include <Misc/String.h>
#include <Graphics/Graphics.h>

#include <SysEvents/SysEvents.h>

namespace PGE {

class WindowOGL3 : public Window {
    public:
        WindowOGL3(String c,int w,int h,bool fs);
        ~WindowOGL3();

        virtual void update();
        virtual void swap(bool vsyncEnabled=true);

        SDL_GLContext getGlContext() const;
    private:
        bool vsync;

        WindowOGL3(){};

        SDL_GLContext glContext;
};

}

#endif

#ifndef PGEINTERNAL_WINDOWOGL3_H_INCLUDED
#define PGEINTERNAL_WINDOWOGL3_H_INCLUDED

#include <SDL.h>

#include <Window/Window.h>
#include "WindowInternal.h"
#include <Misc/String.h>
#include <Graphics/Graphics.h>

#include <SysEvents/SysEvents.h>

namespace PGE {

class WindowOGL3 : public WindowInternal {
    public:
        WindowOGL3(String c,int w,int h,bool fs);
        ~WindowOGL3();

        virtual void update() override;
        virtual void swap(bool vsyncEnabled=true) override;

        SDL_GLContext getGlContext() const;
    private:
        bool vsync;

        WindowOGL3(){};

        void throwException(String func,String details) override;
        void cleanup() override;

        SDL_GLContext glContext;
};

}

#endif

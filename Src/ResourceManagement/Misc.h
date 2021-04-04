#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_SDL
#define PGEINTERNAL_RESOURCEMANAGEMENT_SDL

#include <Misc/String.h>
#include <Exception/Exception.h>

#include <ResourceManagement/Resource.h>
#include <ResourceManagement/ResourceManager.h>

#include <SysEvents/SysEvents.h>

struct SDL_Window;

namespace PGE {

class Graphics;

class WindowEventSubscriber : public Resource<SysEvents::Subscriber*> {
    public:
        WindowEventSubscriber(Graphics* gfx);
        ~WindowEventSubscriber();

        __RES_MNGMT__REF_FACT_METH(WindowEventSubscriber, SysEvents::Subscriber*)
};

class SDLWindow : public Resource<SDL_Window*> {
    public:
        SDLWindow(const String& title, int width, int height, uint32_t flags);
        ~SDLWindow();

        __RES_MNGMT__REF_FACT_METH(SDLWindow, SDL_Window*)
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_SDL
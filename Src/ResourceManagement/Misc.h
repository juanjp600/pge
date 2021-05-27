#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_SDL
#define PGEINTERNAL_RESOURCEMANAGEMENT_SDL

#include <PGE/String/String.h>

#include <PGE/ResourceManagement/ResourceManager.h>

#include <PGE/SysEvents/SysEvents.h>

struct SDL_Window;

namespace PGE {

class Graphics;

class WindowEventSubscriber : public Resource<SysEvents::Subscriber*> {
    public:
        WindowEventSubscriber(Graphics* gfx);
        ~WindowEventSubscriber();
};

class SDLWindow : public Resource<SDL_Window*> {
    public:
        SDLWindow(const String& title, int width, int height, u32 flags);
        ~SDLWindow();
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_SDL
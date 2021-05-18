#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_SDL
#define PGEINTERNAL_RESOURCEMANAGEMENT_SDL

#include <String/String.h>

#include <ResourceManagement/ResourceManager.h>

#include <SysEvents/SysEvents.h>

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
        SDLWindow(const String& title, int width, int height, uint32_t flags);
        ~SDLWindow();
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_SDL
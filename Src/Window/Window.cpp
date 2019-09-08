#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#include <SDL_syswm.h>
#endif

#include <Window/Window.h>
#include "WindowInternal.h"

using namespace PGE;

bool Window::isOpen() const {
    return open;
}

bool Window::isFocused() const {
    return focused;
}

int Window::getWidth() const {
    return width;
}

int Window::getHeight() const {
    return height;
}

SDL_Window* WindowInternal::getSdlWindow() const {
    return sdlWindow;
}

#if defined(__APPLE__) && defined(__OBJC__)
NSWindow* WindowInternal::getCocoaWindow() const {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(getSdlWindow(), &info);

    return info.info.cocoa.window;
}
#endif

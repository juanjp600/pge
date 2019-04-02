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

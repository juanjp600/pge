#include <Window/Window.h>

using namespace PGE;

bool Window::isOpen() const {
    return open;
}

int Window::getWidth() const {
    return width;
}

int Window::getHeight() const {
    return height;
}

SDL_Window* Window::getSdlWindow() const {
    return sdlWindow;
}

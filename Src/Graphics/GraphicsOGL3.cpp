#include <Graphics/Graphics.h>
#include "GraphicsOGL3.h"
#include <Window/Window.h>
#include "../Window/WindowOGL3.h"

using namespace PGE;

Graphics* Graphics::create(int w,int h,bool fs) {
    return new GraphicsOGL3(w,h,fs);
}

GraphicsOGL3::GraphicsOGL3(int w,int h,bool fs) {
    window = new WindowOGL3("PGE",w,h,fs);

    setViewport(Rectanglei(0,0,w,h));
}

GraphicsOGL3::~GraphicsOGL3() {
    delete window;
}

void GraphicsOGL3::update() {
    Graphics::update();
    takeGlContext();
}

void GraphicsOGL3::takeGlContext() {
    if (SDL_GL_GetCurrentContext()!=((WindowOGL3*)window)->getGlContext()) {
        SDL_GL_MakeCurrent(window->getSdlWindow(),((WindowOGL3*)window)->getGlContext());
    }
}

void GraphicsOGL3::setViewport(Rectanglei vp) {
    glViewport(vp.topLeftCorner().x,vp.topLeftCorner().y,vp.width(),vp.height());
}

void GraphicsOGL3::clear(Color color) {
    takeGlContext();

    glClearColor(color.red,color.green,color.blue,color.alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsOGL3.h"
#include <Window/Window.h>
#include "WindowOGL3.h"

using namespace PGE;

WindowOGL3::WindowOGL3(String c,int w,int h,bool fs) {
    caption = c;
    width = w; height = h; fullscreen = fs;

    sdlWindow = SDL_CreateWindow(caption.cstr(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,w,h,SDL_WINDOW_OPENGL /*| SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP*/);
    
//    if (fullscreen) {
//        SDL_SetWindowBordered(sdlWindow,SDL_bool::SDL_FALSE);
//        SDL_Rect displayBounds;
//        int displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
//        SDL_GetDisplayBounds(displayIndex,&displayBounds);
//        SDL_SetWindowSize(sdlWindow,displayBounds.w,displayBounds.h);
//        SDL_SetWindowPosition(sdlWindow,0,0);
//    }

    eventSubscriber = SysEvents::Subscriber(sdlWindow,SysEvents::Subscriber::EventType::WINDOW);
    SysEvents::subscribe(eventSubscriber);

    glContext = SDL_GL_CreateContext(sdlWindow);

    glewExperimental = true;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glClearDepth(1.0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vsync = true;
    SDL_GL_SetSwapInterval(1);

    SDL_GL_SwapWindow(sdlWindow);

    open = true;

    focused = true;
}

WindowOGL3::~WindowOGL3() {
    SysEvents::unsubscribe(eventSubscriber);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
}

SDL_GLContext WindowOGL3::getGlContext() const {
    return glContext;
}

void WindowOGL3::update() {
    SDL_Event event;
    while (eventSubscriber.popEvent(event)) {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            open = false;
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            focused = true;
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            focused = false;
        }
    }
}

void WindowOGL3::swap(bool vsyncEnabled) {
    if (vsyncEnabled != vsync) {
        vsync = vsyncEnabled;
        SDL_GL_SetSwapInterval(vsync ? 1 : 0);
    }
    SDL_GL_SwapWindow(sdlWindow);
}

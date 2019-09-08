#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsOGL3.h"
#include "../SysEvents/SysEventsInternal.h"
#include "../Exception/Exception.h"
#include <Window/Window.h>
#include "WindowOGL3.h"

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#endif

using namespace PGE;

WindowOGL3::WindowOGL3(String c,int w,int h,bool fs) {
    GLuint glError = GL_NO_ERROR;

    caption = c;
    width = w; height = h; fullscreen = fs;

#if defined(__APPLE__) && defined(__OBJC__)
    // Figure out the de-scaled window size.
    NSRect rect = NSMakeRect(0, 0, w, h);
    NSScreen* screen = [NSScreen mainScreen];
    rect = [screen convertRectFromBacking: rect];

    w = NSWidth(rect);
    h = NSHeight(rect);
#endif

    sdlWindow = nullptr;
    sdlWindow = SDL_CreateWindow(caption.cstr(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,w,h,SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI/* | SDL_WINDOW_FULLSCREEN_DESKTOP*/);

    if (sdlWindow == nullptr) {
        throwException("WindowOGL3","Failed to create SDL window: "+String(SDL_GetError()));
    }

//    if (fullscreen) {
//        SDL_SetWindowBordered(sdlWindow,SDL_bool::SDL_FALSE);
//        SDL_Rect displayBounds;
//        int displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
//        SDL_GetDisplayBounds(displayIndex,&displayBounds);
//        SDL_SetWindowSize(sdlWindow,displayBounds.w,displayBounds.h);
//        SDL_SetWindowPosition(sdlWindow,0,0);
//    }

    eventSubscriber = new SysEventsInternal::SubscriberInternal(this,SysEventsInternal::SubscriberInternal::EventType::WINDOW);
    SysEventsInternal::subscribe(eventSubscriber);

    glContext = SDL_GL_CreateContext(sdlWindow);
    // And make it later in the day.
    SDL_GL_MakeCurrent(sdlWindow,glContext);
    SysEventsInternal::update();

    glewExperimental = true;
    glError = glewInit();
    if (glError != GL_NO_ERROR) {
        throwException("WindowOGL3", "Failed to initialize GLEW (GLERROR: " + String(glError, true) + ")");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glClearDepth(1.0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throwException("WindowOGL3", "Failed to initialize window data post-GLEW initialization. (GL_ERROR: " + String(glError, true) + ")");
    }

    vsync = true;
    SDL_GL_SetSwapInterval(1);

    SDL_GL_SwapWindow(sdlWindow);

    open = true;

    focused = true;
}

WindowOGL3::~WindowOGL3() {
    cleanup();
}

void WindowOGL3::cleanup() {
    SysEventsInternal::unsubscribe(eventSubscriber);
    SDL_GL_DeleteContext(glContext);
    if (sdlWindow!=nullptr) { SDL_DestroyWindow(sdlWindow); }
}

void WindowOGL3::throwException(String func, String details) {
    cleanup();
    throw Exception("WindowOGL3::" + func, details);
}

SDL_GLContext WindowOGL3::getGlContext() const {
    return glContext;
}

void WindowOGL3::update() {
    SDL_Event event;
    while (((SysEventsInternal::SubscriberInternal*)eventSubscriber)->popEvent(event)) {
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

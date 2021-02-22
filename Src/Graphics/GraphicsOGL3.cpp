#include "GraphicsOGL3.h"

#include "../Texture/TextureOGL3.h"

using namespace PGE;

#include <iostream>

GraphicsOGL3::OpTakeContext::OpTakeContext(GraphicsOGL3* gfx) {
    graphics = gfx;
}

void GraphicsOGL3::OpTakeContext::exec() {
    graphics->takeGlContext();
}

GraphicsOGL3::GraphicsOGL3(String name, int w, int h, bool fs) : GraphicsInternal(name, w, h, fs) {
    GLenum glError = GL_NO_ERROR;

#if defined(__APPLE__) && defined(__OBJC__)
    // Figure out the de-scaled window size.
    NSRect rect = NSMakeRect(0, 0, w, h);
    NSScreen* screen = [NSScreen mainScreen];
    rect = [screen convertRectFromBacking : rect];

    w = NSWidth(rect);
    h = NSHeight(rect);
#endif

    sdlWindow = SDL_CreateWindow(caption.cstr(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI/* | SDL_WINDOW_FULLSCREEN_DESKTOP*/);

    if (sdlWindow() == nullptr) {
        throw Exception("GraphicsOGL3", "Failed to create SDL window: " + String(SDL_GetError()));
    }

    //    if (fullscreen) {
    //        SDL_SetWindowBordered(sdlWindow,SDL_bool::SDL_FALSE);
    //        SDL_Rect displayBounds;
    //        int displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
    //        SDL_GetDisplayBounds(displayIndex,&displayBounds);
    //        SDL_SetWindowSize(sdlWindow,displayBounds.w,displayBounds.h);
    //        SDL_SetWindowPosition(sdlWindow,0,0);
    //    }

    destructor.setPreop(new GraphicsOGL3::OpTakeContext((GraphicsOGL3*)this));

    glContext = destructor.getReference<SDL_GLContext>([](const SDL_GLContext& c) { SDL_GL_DeleteContext(c); }, SDL_GL_CreateContext(sdlWindow()));
    // And make it later in the day.
    SDL_GL_MakeCurrent(sdlWindow(), glContext());

    glewExperimental = true;
    glError = glewInit();
    if (glError != GL_NO_ERROR) {
        throw Exception("GraphicsOGL3", "Failed to initialize GLEW (GLERROR: " + String::format(glError, "%u") + ")");
    }

    depthTest = true;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glClearDepth(1.0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throw Exception("GraphicsOGL3", "Failed to initialize window data post-GLEW initialization. (GL_ERROR: " + String::format(glError, "%u") + ")");
    }

    SDL_GL_SwapWindow(sdlWindow());

    setViewport(Rectanglei(0,0,w,h));

    glFramebuffer = destructor.getReference<GLuint>([](const GLuint& i) { if (i != GL_INVALID_VALUE) { glBindFramebuffer(GL_FRAMEBUFFER, 0);  glDeleteFramebuffers(1, &i); } }, GL_INVALID_VALUE);
    glGenFramebuffers(1,&glFramebuffer);
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throw Exception("GraphicsOGL3", "Failed to generate frame buffer. (GL_ERROR: " + String::format(glError, "%u") + ")");
    }

    vsync = true;
    SDL_GL_SetSwapInterval(1);
}

void GraphicsOGL3::update() {
    Graphics::update();
    takeGlContext();
}

void GraphicsOGL3::swap() {
    SDL_GL_SwapWindow(sdlWindow());
}

void GraphicsOGL3::takeGlContext() {
    if (SDL_GL_GetCurrentContext()!=glContext()) {
        SDL_GL_MakeCurrent(sdlWindow(),glContext());
    }
}

SDL_GLContext GraphicsOGL3::getGlContext() const {
    return glContext();
}

void GraphicsOGL3::clear(Color color) {
    takeGlContext();

    glDepthMask(GL_TRUE);
    glColorMask(true,true,true,true);
    glClearColor(color.red,color.green,color.blue,color.alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsOGL3::setDepthTest(bool isEnabled) {
    if (isEnabled != depthTest) {
        depthTest = isEnabled;
        if (isEnabled) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }
}

void GraphicsOGL3::setRenderTarget(Texture* renderTarget) {
    takeGlContext();

    glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ((TextureOGL3*)renderTarget)->getGlDepthbuffer());
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ((TextureOGL3*)renderTarget)->getGlTexture(), 0);

    GLenum glAttachments[] = {
        GL_COLOR_ATTACHMENT0
    };
    glDrawBuffers(1, glAttachments);
}

void GraphicsOGL3::setRenderTargets(std::vector<Texture*> renderTargets) {
    takeGlContext();

    TextureOGL3* largestTarget = (TextureOGL3*)renderTargets[0];
    for (int i=0;i<renderTargets.size();i++) {
        if (!renderTargets[i]->isRenderTarget()) {
            throw Exception("setRenderTargets","renderTargets["+String::fromInt(i)+"] is not a valid render target");
        }

        if ((largestTarget->getWidth()+largestTarget->getHeight())<(renderTargets[i]->getWidth()+renderTargets[i]->getHeight())) {
            largestTarget = (TextureOGL3*)renderTargets[i];
        }
    }
    GLenum glAttachments[] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7
    };
    glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, largestTarget->getGlDepthbuffer());
    for (int i=0;i<renderTargets.size();i++) {
        glFramebufferTexture(GL_FRAMEBUFFER, glAttachments[i], ((TextureOGL3*)renderTargets[i])->getGlTexture(), 0);
    }
    glDrawBuffers((GLsizei)renderTargets.size(), glAttachments);
}

void GraphicsOGL3::resetRenderTarget() {
    takeGlContext();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthMask(GL_TRUE);
    glColorMask(true,true,true,true);
}

void GraphicsOGL3::setViewport(Rectanglei vp) {
    if (vp != viewport) {
        viewport = vp;
        glViewport(vp.topLeftCorner().x, vp.topLeftCorner().y, vp.width(), vp.height());
    }
}

void GraphicsOGL3::setVsync(bool isEnabled) {
    if (isEnabled != vsync) {
        vsync = isEnabled;
        SDL_GL_SetSwapInterval(vsync ? 1 : 0);
    }
}

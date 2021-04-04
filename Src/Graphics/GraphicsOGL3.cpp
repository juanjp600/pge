#include "GraphicsOGL3.h"

#include "../Texture/TextureOGL3.h"

using namespace PGE;

GraphicsOGL3::GraphicsOGL3(String name, int w, int h, bool fs) : GraphicsInternal(name, w, h, fs, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI/* | SDL_WINDOW_FULLSCREEN_DESKTOP*/), resourceManager(this, 2) {
#if defined(__APPLE__) && defined(__OBJC__)
    // Figure out the de-scaled window size.
    NSRect rect = NSMakeRect(0, 0, w, h);
    NSScreen* screen = [NSScreen mainScreen];
    rect = [screen convertRectFromBacking : rect];

    w = NSWidth(rect);
    h = NSHeight(rect);
#endif

    //    if (fullscreen) {
    //        SDL_SetWindowBordered(sdlWindow,SDL_bool::SDL_FALSE);
    //        SDL_Rect displayBounds;
    //        int displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
    //        SDL_GetDisplayBounds(displayIndex,&displayBounds);
    //        SDL_SetWindowSize(sdlWindow,displayBounds.w,displayBounds.h);
    //        SDL_SetWindowPosition(sdlWindow,0,0);
    //    }

    glContext = GLContext::createRef(resourceManager, sdlWindow());

    glewExperimental = true;
    if (glewInit() != GL_NO_ERROR) {
        throw Exception("GraphicsOGL3", "Failed to initialize GLEW (GLERROR: " + String::format(glGetError(), "%u") + ")");
    }

    depthTest = true;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glClearDepth(1.0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throw Exception("GraphicsOGL3", "Failed to initialize window data post-GLEW initialization. (GL_ERROR: " + String::format(glError, "%u") + ")");
    }

    SDL_GL_SwapWindow(sdlWindow);

    setViewport(Rectanglei(0,0,w,h));

    glFramebuffer = GLFramebuffer::createRef(resourceManager);

    SDL_GL_SetSwapInterval(1);
}

void GraphicsOGL3::update() {
    Graphics::update();
    takeGlContext();
}

void GraphicsOGL3::swap() {
    SDL_GL_SwapWindow(sdlWindow);
}

void GraphicsOGL3::takeGlContext() {
    if (SDL_GL_GetCurrentContext()!=glContext) {
        SDL_GL_MakeCurrent(sdlWindow,glContext);
    }
}

SDL_GLContext GraphicsOGL3::getGlContext() const {
    return glContext;
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

    glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer);
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
    for (int i = 0; i < (int)renderTargets.size(); i++) {
        if (!renderTargets[i]->isRenderTarget()) {
            throw Exception("setRenderTargets","renderTargets["+String::fromInt(i)+"] is not a valid render target");
        }

        if (i == 0) { continue; }

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
    glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, largestTarget->getGlDepthbuffer());
    for (int i = 0; i < (int)renderTargets.size(); i++) {
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
        takeGlContext();

        viewport = vp;
        glViewport(vp.topLeftCorner().x, vp.topLeftCorner().y, vp.width(), vp.height());
    }
}

void GraphicsOGL3::setVsync(bool isEnabled) {
    if (isEnabled != vsync) {
        takeGlContext();

        vsync = isEnabled;
        SDL_GL_SetSwapInterval(vsync ? 1 : 0);
    }
}

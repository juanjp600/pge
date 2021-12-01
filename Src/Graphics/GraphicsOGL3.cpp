#include "GraphicsOGL3.h"

#include <glad/gl.h>

using namespace PGE;

GraphicsOGL3::GraphicsOGL3(const String& name, int w, int h, WindowMode wm, int x, int y)
    //TODO: this is incorrect on macOS
    : GraphicsSpecialized(name, w, h, wm, x, y, SDL_WINDOW_OPENGL), resourceManager(*this) {
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

    glContext = resourceManager.addNewResource<GLContext>(getWindow());

    asrt(gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) != 0, "Failed to initialize GLAD (GLERROR: " + String::from(glGetError()) + ")");

    depthTest = true;
    glEnable(GL_DEPTH_TEST);
    cullingMode = CullingMode::NONE;
    setCulling(CullingMode::BACK);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glClearDepth(1.0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLenum glError = glGetError();
    asrt(glError == GL_NO_ERROR, "Failed to initialize window data post-GLAD initialization (GLERROR: " + String::from(glError) + ")");

    SDL_GL_SwapWindow(getWindow());

    setViewport(Rectanglei(0,0,w,h));

    glFramebuffer = resourceManager.addNewResource<GLFramebuffer>();

    vsync = true;
    SDL_GL_SetSwapInterval(1);

    updateRenderTargetFlags(false);
}

void GraphicsOGL3::update() {
    Graphics::update();
    takeGlContext();
}

void GraphicsOGL3::swap() {
    SDL_GL_SwapWindow(getWindow());
}

void GraphicsOGL3::takeGlContext() {
    if (SDL_GL_GetCurrentContext()!=glContext) {
        SDL_GL_MakeCurrent(getWindow(),glContext);
    }
}

SDL_GLContext GraphicsOGL3::getGlContext() const {
    return glContext;
}

void GraphicsOGL3::clear(const Color& color) {
    takeGlContext();

    glDepthMask(GL_TRUE);
    glColorMask(true,true,true,true);
    glClearColor(color.red,color.green,color.blue,color.alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsOGL3::setRenderTarget(Texture& renderTarget) {
    updateRenderTargetFlags(true);

    takeGlContext();

    glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ((TextureOGL3&)renderTarget).getGlDepthbuffer());
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ((TextureOGL3&)renderTarget).getGlTexture(), 0);

    GLenum glAttachments[] = {
        GL_COLOR_ATTACHMENT0
    };
    glDrawBuffers(1, glAttachments);
}

void GraphicsOGL3::setRenderTargets(const ReferenceVector<Texture>& renderTargets) {
    updateRenderTargetFlags(true);

    takeGlContext();

    TextureOGL3* largestTarget = &(TextureOGL3&)renderTargets[0].get();
    for (size_t i : Range(renderTargets.size())) {
        TextureOGL3& rt = (TextureOGL3&)renderTargets[i].get();

        asrt(rt.isRenderTarget(), "renderTargets[" + String::from(i) + "] is not a valid render target");

        if (i == 0) { continue; }

        if ((largestTarget->getWidth() + largestTarget->getHeight()) < (rt.getWidth() + rt.getHeight())) {
            largestTarget = &rt;
        }
    }

    constexpr GLenum glAttachments[] = {
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
    for (size_t i : Range(renderTargets.size())) {
        glFramebufferTexture(GL_FRAMEBUFFER, glAttachments[i], ((TextureOGL3&)renderTargets[i].get()).getGlTexture(), 0);
    }
    glDrawBuffers((GLsizei)renderTargets.size(), glAttachments);
}

void GraphicsOGL3::resetRenderTarget() {
    updateRenderTargetFlags(false);

    takeGlContext();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthMask(GL_TRUE);
    glColorMask(true,true,true,true);
}

void GraphicsOGL3::setViewport(const Rectanglei& vp) {
    if (vp != viewport) {
        takeGlContext();

        viewport = vp;
        glViewport(vp.topLeftCorner().x, dimensions.y - vp.topLeftCorner().y - vp.height(), vp.width(), vp.height());
    }
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

void GraphicsOGL3::setVsync(bool isEnabled) {
    if (isEnabled != vsync) {
        takeGlContext();

        vsync = isEnabled;
        SDL_GL_SetSwapInterval(vsync ? 1 : 0);
    }
}

void GraphicsOGL3::setCulling(CullingMode mode) {
    if (mode == cullingMode) { return; }

    if (mode == CullingMode::NONE) {
        glDisable(GL_CULL_FACE);
    } else {
        if (cullingMode == CullingMode::NONE) {
            glEnable(GL_CULL_FACE);
        }
        updateCullingMode(mode, renderingToRenderTarget);
    }

    cullingMode = mode;
}

void GraphicsOGL3::updateCullingMode(CullingMode newMode, bool flip) {
    if (newMode == CullingMode::NONE) { return; }

    GLenum glMode;
    if ((newMode == CullingMode::BACK) ^ flip) {
        glMode = GL_BACK;
    } else {
        glMode = GL_FRONT;
    }
    glCullFace(glMode);
}

void GraphicsOGL3::addRenderTargetFlag(Shader::Constant& c) {
    renderTargetFlags.emplace(&c);
}

void GraphicsOGL3::removeRenderTargetFlag(Shader::Constant& c) {
    renderTargetFlags.erase(&c);
}

void GraphicsOGL3::updateRenderTargetFlags(bool rt) {
    updateCullingMode(cullingMode, rt);

    float yFlip = rt ? -1.f : 1.f;
    for (Shader::Constant* c : renderTargetFlags) {
        c->setValue(yFlip);
    }

    renderingToRenderTarget = rt;
}

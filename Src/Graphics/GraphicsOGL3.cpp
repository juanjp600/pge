#include <Graphics/Graphics.h>
#include "GraphicsOGL3.h"
#include <Window/Window.h>
#include "../Window/WindowOGL3.h"
#include <Texture/Texture.h>
#include "../Texture/TextureOGL3.h"

using namespace PGE;

Graphics* Graphics::create(String name,int w,int h,bool fs) {
    return new GraphicsOGL3(name,w,h,fs);
}

GraphicsOGL3::GraphicsOGL3(String name,int w,int h,bool fs) {
    window = new WindowOGL3(name,w,h,fs);

    setViewport(Rectanglei(0,0,w,h));

    glGenFramebuffers(1,&glFramebuffer);
}

GraphicsOGL3::~GraphicsOGL3() {
    takeGlContext();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1,&glFramebuffer);

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

    glDepthMask(GL_TRUE);
    glColorMask(true,true,true,true);
    glClearColor(color.red,color.green,color.blue,color.alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    for (int i=0;i<renderTargets.size();i++) {
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
    for (int i=0;i<renderTargets.size();i++) {
        glFramebufferTexture(GL_FRAMEBUFFER, glAttachments[i], ((TextureOGL3*)renderTargets[i])->getGlTexture(), 0);
    }
    glDrawBuffers(renderTargets.size(), glAttachments);
}

void GraphicsOGL3::resetRenderTarget() {
    takeGlContext();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

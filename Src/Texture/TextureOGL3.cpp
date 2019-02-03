#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsOGL3.h"
#include <Texture/Texture.h>
#include "TextureInternal.h"
#include "TextureOGL3.h"

#include <stdlib.h>
#include <inttypes.h>

using namespace PGE;

Texture* Texture::create(Graphics* gfx,int w,int h,bool renderTarget,const void* buffer,Texture::FORMAT fmt) {
    return new TextureOGL3(gfx,w,h,renderTarget,buffer,fmt);
}

Texture* Texture::load(Graphics* gfx,String fn) {
    return new TextureOGL3(gfx,fn);
}

TextureOGL3::TextureOGL3(Graphics* gfx,int w,int h,bool renderTarget,const void* buffer,Texture::FORMAT fmt) {
    graphics = gfx; ((GraphicsOGL3*)graphics)->takeGlContext();

    format = fmt;

    filename = "<n/a>";

    uint8_t* newBuffer = nullptr;

    if (renderTarget) {
        name = "RenderTarget";
        buffer = nullptr;
    } else {
        name = "Static";
        if (buffer==nullptr) {
            name = String(name,"_Blank");
            newBuffer = new uint8_t[width*height*4];
            memset(newBuffer,0,width*height*4*sizeof(uint8_t));
            buffer = newBuffer;
        } else {
            name = String(name,"_Buffer");
        }
    }

    width = w; height = h;
    realWidth = width; realHeight = height;

    glGenTextures(1,&glTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,glTexture);

    GLint glInternalFormat;
    GLenum glFormat;
    GLenum glPixelType;
    switch (format) {
        case FORMAT::RGBA32: {
            glInternalFormat = GL_RGBA;
            glFormat = GL_RGBA;
            glPixelType = GL_UNSIGNED_BYTE;
        } break;
        case FORMAT::R32F: {
            glInternalFormat = GL_R32F;
            glFormat = GL_RED;
            glPixelType = GL_FLOAT;
        } break;
    }

    glTexImage2D(GL_TEXTURE_2D,0,glInternalFormat,realWidth,realHeight,0,glFormat,glPixelType,buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, renderTarget ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, renderTarget ? GL_NEAREST : GL_LINEAR);

    isRT = renderTarget;
    if (isRT) {
        /*glGenFramebuffers(1,&glFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer);*/

        glGenRenderbuffers(1, &glDepthbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, glDepthbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthbuffer);
    }

    if (newBuffer!=nullptr) { delete[] newBuffer; }

    opaque = true;
}

TextureOGL3::TextureOGL3(Graphics* gfx,const String& fn) {
    graphics = gfx; ((GraphicsOGL3*)graphics)->takeGlContext();

    filename = fn;
    name = fn;

    BYTE* fiBuffer = loadFIBuffer(filename,width,height,realWidth,realHeight,opaque);

    glGenTextures(1,&glTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,glTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,realWidth,realHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,fiBuffer);

    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] fiBuffer;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

    isRT = false;
}

TextureOGL3::~TextureOGL3() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    if (isRT) {
        //glDeleteFramebuffers(1,&glFramebuffer);
        glDeleteRenderbuffers(1,&glDepthbuffer);
    }
    glDeleteTextures(1,&glTexture);
}

bool TextureOGL3::isRenderTarget() const {
    return isRT;
}

GLuint TextureOGL3::getGlTexture() const {
    return glTexture;
}

/*GLuint TextureOGL3::getGlFramebuffer() const {
    return glFramebuffer;
}*/

GLuint TextureOGL3::getGlDepthbuffer() const {
    return glDepthbuffer;
}

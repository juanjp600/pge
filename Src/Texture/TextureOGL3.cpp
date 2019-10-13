#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsOGL3.h"
#include <Texture/Texture.h>
#include "TextureInternal.h"
#include "TextureOGL3.h"
#include "../Exception/Exception.h"

#include <stdlib.h>
#include <inttypes.h>

using namespace PGE;

Texture* Texture::create(Graphics* gfx,int w,int h,bool renderTarget,const void* buffer,Texture::FORMAT fmt) {
    return new TextureOGL3(gfx,w,h,renderTarget,buffer,fmt);
}

Texture* Texture::load(Graphics* gfx,FileName fn) {
    return new TextureOGL3(gfx,fn);
}

Texture* Texture::load(Graphics* gfx,FileName fn,ThreadManager* threadManager) {
    return new TextureOGL3(gfx,fn,threadManager);
}

TextureOGL3::TextureOGL3(Graphics* gfx,int w,int h,bool renderTarget,const void* buffer,Texture::FORMAT fmt) {
    GLuint glError = GL_NO_ERROR;
    
    graphics = gfx; ((GraphicsOGL3*)graphics)->takeGlContext();

    format = fmt;

//    filename = "<n/a>";

    uint8_t* newBuffer = nullptr;

    width = w; height = h;
    realWidth = width; realHeight = height;
    
    if (renderTarget) {
        name = "RenderTarget";
        buffer = nullptr;
    } else {
        name = "Static";
        if (buffer==nullptr) {
            name = String(name,"_Blank");
            newBuffer = new uint8_t[(size_t)realWidth*(size_t)realHeight*4];
            memset(newBuffer,0,(size_t)realWidth*(size_t)realHeight*4*sizeof(uint8_t));
            buffer = newBuffer;
        } else {
            name = String(name,"_Buffer");
        }
    }

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
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throwException("TextureOGL3(w,h,rt)", "Failed to create texture ("+String(realWidth)+","+String(realHeight)+"; GLERROR "+String(glError, true)+")");
    }

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
}

TextureOGL3::TextureOGL3(Graphics* gfx,const FileName& fn) {
    GLuint glError = GL_NO_ERROR;
    
    graphics = gfx; ((GraphicsOGL3*)graphics)->takeGlContext();

    filename = fn;
    name = fn.str();

    BYTE* fiBuffer = loadFIBuffer(filename,width,height,realWidth,realHeight);

    glGenTextures(1,&glTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,glTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,realWidth,realHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,fiBuffer);
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throwException("TextureOGL3(fn)", "Failed to create texture (filename: "+filename.str()+"; GLERROR "+String(glError, true)+")");
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] fiBuffer;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

    isRT = false;
}

TextureOGL3::TextureOGL3(Graphics* gfx,const FileName& fn,ThreadManager* threadManager) {
    GLuint glError = GL_NO_ERROR;
    
    graphics = gfx; ((GraphicsOGL3*)graphics)->takeGlContext();

    filename = fn;
    name = fn.str();

    glGenTextures(1,&glTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,glTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,realWidth,realHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throwException("TextureOGL3(fn,threadMgr)", "Failed to create texture (filename: "+filename.str()+"; GLERROR "+String(glError, true)+")");
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

    isRT = false;

    class TextureReassignRequest : public ThreadManager::MainThreadRequest {
        public:
            FileName filename;
            GraphicsOGL3* graphics;
            GLuint glTexture;

            int realWidth; int realHeight;
            BYTE* buffer;

            void execute() override {
                GLuint glError = GL_NO_ERROR;
                
                graphics->takeGlContext();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,glTexture);
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,realWidth,realHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,buffer);
                glError = glGetError();
                if (glError != GL_NO_ERROR) {
                    throw Exception("TextureReassignRequest (OGL3)", "Failed to create texture (filename: "+filename.str()+"; GLERROR "+String(glError, true)+")");
                }

                glGenerateMipmap(GL_TEXTURE_2D);
            }
    } mainThreadRequest;
    mainThreadRequest.filename = filename;
    mainThreadRequest.graphics = (GraphicsOGL3*)graphics;
    mainThreadRequest.glTexture = glTexture;

    class TextureLoadRequest : public ThreadManager::NewThreadRequest {
        public:
            TextureReassignRequest mainThreadRequest;
            FileName filename;
            int* width; int* height; int* realWidth; int* realHeight;
            void execute() {
                BYTE* fiBuffer = loadFIBuffer(filename,*width,*height,*realWidth,*realHeight);

                mainThreadRequest.realWidth = *realWidth;
                mainThreadRequest.realHeight = *realHeight;
                mainThreadRequest.buffer = fiBuffer;
                requestExecutionOnMainThread(&mainThreadRequest);

                delete[] fiBuffer;

                markAsDone();
            }
    };

    TextureLoadRequest* textureLoadRequest = new TextureLoadRequest();
    textureLoadRequest->mainThreadRequest = mainThreadRequest;
    textureLoadRequest->filename = filename;
    textureLoadRequest->width = &width;
    textureLoadRequest->height = &height;
    textureLoadRequest->realWidth = &realWidth;
    textureLoadRequest->realHeight = &realHeight;

    threadManager->requestExecutionOnNewThread(textureLoadRequest);
}

TextureOGL3::~TextureOGL3() {
    cleanup();
}

void TextureOGL3::throwException(String func, String details) {
    cleanup();
    throw Exception("TextureOGL3::" + func, details);
}

void TextureOGL3::cleanup() {
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

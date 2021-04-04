#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsOGL3.h"
#include <Texture/Texture.h>
#include "TextureInternal.h"
#include "TextureOGL3.h"
#include <Exception/Exception.h>

using namespace PGE;

TextureOGL3::TextureOGL3(Graphics* gfx,int w,int h,bool renderTarget,const void* buffer,Texture::FORMAT fmt) : resourceManager(gfx, 2) {
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

    glTexture = GLTexture::createRef(resourceManager);
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
        throw Exception("TextureOGL3(w,h,rt)", "Failed to create texture ("+String::fromInt(realWidth)+","+String::fromInt(realHeight)+"; GLERROR "+String::format(glError, "%u")+")");
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, renderTarget ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, renderTarget ? GL_NEAREST : GL_LINEAR);

    isRT = renderTarget;
    if (isRT) {
        /*glGenFramebuffers(1,&glFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer);*/

        glDepthbuffer = GLDepthBuffer::createRef(resourceManager, w, h);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthbuffer);
    }

    if (newBuffer!=nullptr) { delete[] newBuffer; }
}

TextureOGL3::TextureOGL3(Graphics* gfx, uint8_t* fiBuffer, int w, int h, int rw, int rh, const FilePath& fn) : resourceManager(gfx, 1) {
    graphics = gfx;
    width = w;
    height = h;
    realWidth = rw;
    realHeight = rh;
    filename = fn;
    name = fn.str();

    GLuint glError = GL_NO_ERROR;

    ((GraphicsOGL3*)graphics)->takeGlContext();

    glTexture = GLTexture::createRef(resourceManager);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,glTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,realWidth,realHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,fiBuffer);
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throw Exception("TextureOGL3(fn)", "Failed to create texture (filename: "+filename.str()+"; GLERROR "+String::format(glError, "%u")+")");
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

    isRT = false;
}

TextureOGL3::TextureOGL3(Graphics* gfx,const FilePath& fn,ThreadManager* threadManager) : resourceManager(gfx, 1) {
    GLuint glError = GL_NO_ERROR;

    graphics = gfx; ((GraphicsOGL3*)graphics)->takeGlContext();

    filename = fn;
    name = fn.str();

    glTexture = GLTexture::createRef(resourceManager);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,glTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,realWidth,realHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    glError = glGetError();
    if (glError != GL_NO_ERROR) {
        throw Exception("TextureOGL3(fn,threadMgr)", "Failed to create texture (filename: "+filename.str()+"; GLERROR "+String::format(glError, "%u")+")");
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

    isRT = false;

    class TextureReassignRequest : public ThreadManager::MainThreadRequest {
        public:
            FilePath filename;
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
                    throw Exception("TextureReassignRequest (OGL3)", "Failed to create texture (filename: "+filename.str()+"; GLERROR "+String::format(glError, "%u")+")");
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
            FilePath filename;
            int* width; int* height; int* realWidth; int* realHeight;
            void execute() override {
                BYTE* fiBuffer = loadFIBufferFromFile(filename,*width,*height,*realWidth,*realHeight);

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

// TODO: Test.
Texture* TextureOGL3::copy() const {
    TextureOGL3* copy = new TextureOGL3(graphics, width, height, false, nullptr, format);
    copy->name = name + "_Copy";
    
    glCopyImageSubData(getGlTexture(), GL_TEXTURE_2D, 0, 0, 0, 0, copy->getGlTexture(), GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 0);

    return copy;
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

void* TextureOGL3::getNative() const {
    return (void*)&glTexture;
}

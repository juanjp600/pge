#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsOGL3.h"
#include <Texture/Texture.h>
#include "TextureInternal.h"
#include "TextureOGL3.h"

#include <stdlib.h>
#include <inttypes.h>

using namespace PGE;

TextureOGL3::TextureOGL3(Graphics* gfx,int w,int h,bool renderTarget,const void* buffer) {
    graphics = gfx; ((GraphicsOGL3*)graphics)->takeGlContext();

    filename = "<n/a>";

    void* newBuffer = nullptr;

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
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,realWidth,realHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, renderTarget ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, renderTarget ? GL_NEAREST : GL_LINEAR);

    isRT = renderTarget;
    /*if (isRT) {
        glGenFramebuffers(1,&glFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer);
        TODO: FINISH THIS
    }*/

    if (newBuffer!=nullptr) { delete[] newBuffer; }
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

    delete[] fiBuffer;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    isRT = false;
}

TextureOGL3::~TextureOGL3() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    if (isRT) {
        glDeleteFramebuffers(1,&glFramebuffer);
    }
    glDeleteTextures(1,&glTexture);
}

bool TextureOGL3::isRenderTarget() const {
    return isRT;
}

#include <PGE/Graphics/Graphics.h>
#include "../GraphicsOGL3.h"
#include <PGE/Graphics/Texture.h>
#include "TextureOGL3.h"
#include <PGE/Exception/Exception.h>

using namespace PGE;

static void textureImage(int width, int height, const byte* buffer, Texture::Format format) {
    GLint glInternalFormat;
    GLenum glFormat;
    GLenum glPixelType;
    switch (format) {
        case Texture::Format::RGBA32: {
            glInternalFormat = GL_RGBA;
            glFormat = GL_RGBA;
            glPixelType = GL_UNSIGNED_BYTE;
        } break;
        case Texture::Format::R32F: {
            glInternalFormat = GL_R32F;
            glFormat = GL_RED;
            glPixelType = GL_FLOAT;
        } break;
        case Texture::Format::R8: {
            glInternalFormat = GL_R8;
            glFormat = GL_RED;
            glPixelType = GL_UNSIGNED_BYTE;
        } break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, glFormat, glPixelType, buffer);
    GLenum glError = glGetError();
    PGE_ASSERT(glError == GL_NO_ERROR, "Failed to create texture (" + String::fromInt(width) + "x" + String::fromInt(height) + "; GLERROR: " + String::format(glError, "%u") + ")");
}

static void applyTextureParameters(bool rt) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, rt ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, rt ? GL_NEAREST : GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, rt ? 1.f : 4.f);
}

TextureOGL3::TextureOGL3(Graphics* gfx, int w, int h, Format fmt) : Texture(gfx, w, h, true, fmt), resourceManager(gfx, 2) {
    ((GraphicsOGL3*)gfx)->takeGlContext();
    glTexture = resourceManager.takeOwnership(new GLTexture());
    textureImage(w, h, nullptr, fmt);
    applyTextureParameters(true);
    /*glGenFramebuffers(1,&glFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer);*/
    glDepthbuffer = resourceManager.takeOwnership(new GLDepthBuffer(w, h));
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthbuffer);
}

TextureOGL3::TextureOGL3(Graphics* gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps) : Texture(gfx, w, h, false, fmt), resourceManager(gfx, 2) {
    ((GraphicsOGL3*)gfx)->takeGlContext();
    glTexture = resourceManager.takeOwnership(new GLTexture());
    textureImage(w, h, buffer, fmt);
    if (mipmaps) { glGenerateMipmap(GL_TEXTURE_2D); }
    applyTextureParameters(false);
}

// TODO: Look at this again.
Texture* TextureOGL3::copy() const {
    //TextureOGL3* copy = new TextureOGL3(graphics, width, height, format);

    //glCopyImageSubData(getGlTexture(), GL_TEXTURE_2D, 0, 0, 0, 0, copy->getGlTexture(), GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 0);

    return nullptr;
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

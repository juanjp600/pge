#include "../GraphicsOGL3.h"

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
        default: {
            throw PGE_CREATE_EX("Invalid format");
        }
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

TextureOGL3::TextureOGL3(Graphics& gfx, int w, int h, Format fmt) : Texture(w, h, true, fmt), resourceManager((GraphicsOGL3&)gfx) {
    ((GraphicsOGL3&)gfx).takeGlContext();
    glTexture = resourceManager.addNewResource<GLTexture>();
    textureImage(w, h, nullptr, fmt);
    applyTextureParameters(true);
    /*glGenFramebuffers(1,&glFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer);*/
    glDepthbuffer = resourceManager.addNewResource<GLDepthBuffer>(w, h);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDepthbuffer);
}

TextureOGL3::TextureOGL3(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps) : Texture(w, h, false, fmt), resourceManager((GraphicsOGL3&)gfx) {
    ((GraphicsOGL3&)gfx).takeGlContext();
    glTexture = resourceManager.addNewResource<GLTexture>();
    textureImage(w, h, buffer, fmt);
    if (mipmaps) { glGenerateMipmap(GL_TEXTURE_2D); }
    applyTextureParameters(false);
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

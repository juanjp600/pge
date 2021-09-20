#include "../GraphicsOGL3.h"
#include <stdlib.h>

using namespace PGE;

static int getCompressedFormat(Texture::CompressedFormat fmt) {
    switch (fmt) {
        case Texture::CompressedFormat::BC1: {
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        }
        case Texture::CompressedFormat::BC2: {
            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        }
        case Texture::CompressedFormat::BC3: {
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        }
        case Texture::CompressedFormat::BC4: {
            return GL_COMPRESSED_RED_RGTC1;
        }
        case Texture::CompressedFormat::BC5: {
            return GL_COMPRESSED_RG_RGTC2;
        }
    }
}

static void textureImage(int width, int height, const byte* buffer, Texture::Format fmt) {
    GLint glInternalFormat;
    GLenum glFormat;
    GLenum glPixelType;
    switch (fmt) {
        case Texture::Format::RGBA64: {
            glInternalFormat = GL_RGBA16;
            glFormat = GL_RGBA;
            glPixelType = GL_UNSIGNED_SHORT;
        } break;
        case Texture::Format::RGBA32: {
            glInternalFormat = GL_RGBA8;
            glFormat = GL_RGBA;
            glPixelType = GL_UNSIGNED_BYTE;
        } break;
        case Texture::Format::R32F: {
            glInternalFormat = GL_DEPTH_COMPONENT32F;
            glFormat = GL_DEPTH_COMPONENT;
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
    PGE_ASSERT(glError == GL_NO_ERROR, "Failed to create texture (" + String::from(width) + "x" + String::from(height) + "; GLERROR: " + String::from(glError) + ")");
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

TextureOGL3::TextureOGL3(const Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt) : Texture(mipmaps[0].width, mipmaps[0].height, false, fmt), resourceManager((GraphicsOGL3&)gfx) {
    ((GraphicsOGL3&)gfx).takeGlContext();
    glTexture = resourceManager.addNewResource<GLTexture>();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)(mipmaps.size() - 1));
    for (int i = 0; i < mipmaps.size(); i++) {
        // TODO: A flip.
        glCompressedTexImage2D(GL_TEXTURE_2D, i, getCompressedFormat(fmt),
            mipmaps[i].width, mipmaps[i].height, 0, (GLsizei)mipmaps[i].size, mipmaps[i].buffer);
    }
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

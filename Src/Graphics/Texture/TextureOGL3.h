#ifndef PGEINTERNAL_TEXTUREOGL3_H_INCLUDED
#define PGEINTERNAL_TEXTUREOGL3_H_INCLUDED

#include <PGE/Graphics/Texture.h>

#include "../../ResourceManagement/OGL3.h"
#include "../../ResourceManagement/ResourceManagerOGL3.h"

namespace PGE {

class TextureOGL3 : public Texture {
    public:
        // Render target.
        TextureOGL3(Graphics& gfx, int w, int h, Format fmt);
        // Loaded texture.
        TextureOGL3(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps);
        // Loaded, compressed texture.
        TextureOGL3(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt);

        GLuint getGlTexture() const;
        //GLuint getGlFramebuffer() const;
        GLuint getGlDepthbuffer() const;
        void* getNative() const override;

    private:
        GLTexture::View glTexture;
        //GLuint glFramebuffer;
        GLDepthBuffer::View glDepthbuffer;

        ResourceManagerOGL3 resourceManager;
};

}

#endif // PGEINTERNAL_TEXTUREOGL3_H_INCLUDED

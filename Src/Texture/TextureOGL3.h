#ifndef PGEINTERNAL_TEXTUREOGL3_H_INCLUDED
#define PGEINTERNAL_TEXTUREOGL3_H_INCLUDED

#include <Texture/Texture.h>
#include <Threading/ThreadManager.h>

#include "../ResourceManagement/OGL3.h"
#include "../ResourceManagement/ResourceManagerOGL3.h"

namespace PGE {

class TextureOGL3 : public Texture {
    public:
        // Render target.
        TextureOGL3(Graphics* gfx, int w, int h, Format fmt);
        // Loaded texture.
        TextureOGL3(Graphics* gfx, int w, int h, byte* buffer, Format fmt);

        Texture* copy() const override;

        GLuint getGlTexture() const;
        //GLuint getGlFramebuffer() const;
        GLuint getGlDepthbuffer() const;
        void* getNative() const override;

    private:
        GLTexture::Ref glTexture;
        //GLuint glFramebuffer;
        GLDepthBuffer::Ref glDepthbuffer;

        ResourceManagerOGL3 resourceManager;
};

}

#endif // PGEINTERNAL_TEXTUREOGL3_H_INCLUDED

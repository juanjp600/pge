#ifndef PGEINTERNAL_TEXTURE_VK_H_INCLUDED
#define PGEINTERNAL_TEXTURE_VK_H_INCLUDED

#include <Texture/Texture.h>

namespace PGE {

class TextureVK : public Texture {
    public:
        // Render target.
        TextureVK(Graphics* gfx, int w, int h, Format fmt);
        // Loaded texture.
        TextureVK(Graphics* gfx, int w, int h, uint8_t* buffer, Format fmt);

        Texture* copy() const override;

        void* getNative() const override;
};

}

#endif // PGEINTERNAL_TEXTURE_VK_H_INCLUDED

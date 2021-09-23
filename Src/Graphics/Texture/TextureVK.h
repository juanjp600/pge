#ifndef PGEINTERNAL_TEXTURE_VK_H_INCLUDED
#define PGEINTERNAL_TEXTURE_VK_H_INCLUDED

#include <PGE/Graphics/Texture.h>

namespace PGE {

class TextureVK : public Texture {
    public:
        // Render target.
        TextureVK(Graphics& gfx, int w, int h, Format fmt);
        // Loaded texture.
        TextureVK(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps);
        // Loaded, compressed texture.
        TextureVK(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt);

        void* getNative() const override;
};

}

#endif // PGEINTERNAL_TEXTURE_VK_H_INCLUDED

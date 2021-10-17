#ifndef PGEINTERNAL_TEXTURE_VK_H_INCLUDED
#define PGEINTERNAL_TEXTURE_VK_H_INCLUDED

#include <PGE/Graphics/Texture.h>

namespace PGE {

class TextureVK : public Texture {
    public:
        static vk::Format getFormat(Texture::Format fmt);
        static vk::Format getFormat(Texture::CompressedFormat fmt);
        static vk::Format getFormat(const Texture::AnyFormat& fmt);

        // Loaded texture.
        TextureVK(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps);
        // Loaded, compressed texture.
        TextureVK(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt);

        // Internal usage, depth buffer.
        TextureVK(Graphics& gfx, int w, int h);

        const vk::ImageView getImageView() const;

        void* getNative() const override;

    protected:
        ResourceManagerVK resourceManager;

        VKImage::View image;
        VKMemory::View imageMem;

        VKImageView::View imageView; // Great type name!

        // Internal usage, render texture.
        TextureVK(Graphics& gfx, int w, int h, Format fmt) : Texture(w, h, true, fmt), resourceManager(gfx) { }
};

}

#endif // PGEINTERNAL_TEXTURE_VK_H_INCLUDED

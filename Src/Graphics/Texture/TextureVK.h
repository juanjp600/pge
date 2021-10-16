#ifndef PGEINTERNAL_TEXTURE_VK_H_INCLUDED
#define PGEINTERNAL_TEXTURE_VK_H_INCLUDED

#include <PGE/Graphics/Texture.h>

#include <PGE/ResourceManagement/RawWrapper.h>

namespace PGE {

class TextureVK : public Texture {
    public:
        // Render target.
        TextureVK(Graphics& gfx, int w, int h, Format fmt);
        // Loaded texture.
        TextureVK(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps);
        // Loaded, compressed texture.
        TextureVK(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt);

        // Internal usage, depth buffer.
        TextureVK(Graphics& gfx, int w, int h);

        const vk::ImageView getImageView() const;
        const vk::RenderPass getRenderPass() const;
        const vk::Framebuffer getFramebuffer() const;

        void* getNative() const override;

    private:
        ResourceManagerVK resourceManager;

        VKImage::View image;
        VKMemory::View imageMem;

        VKImageView::View imageView; // Great type name!

        VKRenderPass::View renderPass;
        RawWrapper<TextureVK>::View depth;
        VKFramebuffer::View framebuffer;
};

}

#endif // PGEINTERNAL_TEXTURE_VK_H_INCLUDED

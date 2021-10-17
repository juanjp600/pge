#ifndef PGEINTERNAL_RENDERTEXTURE_VK_H_INCLUDED
#define PGEINTERNAL_RENDERTEXTURE_VK_H_INCLUDED

#include "TextureVK.h"

#include <PGE/ResourceManagement/RawWrapper.h>

namespace PGE {

class RenderTextureVK : public TextureVK {
    public:
        RenderTextureVK(Graphics& gfx, int w, int h, Format fmt);
        ~RenderTextureVK();

        const vk::RenderPass getRenderPass() const;
        const vk::Framebuffer getFramebuffer() const;

        vk::Format getFormat() const;
        const vk::Rect2D& getScissor() const;

    private:
        GraphicsVK& graphics;
        vk::Format format;

        vk::RenderPass renderPass;

        RawWrapper<TextureVK>::View depth;
        VKFramebuffer::View framebuffer;

        vk::Rect2D scissor;
};

}

#endif // PGEINTERNAL_RENDERTEXTURE_VK_H_INCLUDED

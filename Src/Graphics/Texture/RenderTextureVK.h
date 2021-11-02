#ifndef PGEINTERNAL_RENDERTEXTURE_VK_H_INCLUDED
#define PGEINTERNAL_RENDERTEXTURE_VK_H_INCLUDED

#include "TextureVK.h"

#include <PGE/ResourceManagement/RawWrapper.h>

namespace PGE {

class RenderTextureVK : public TextureVK {
    public:
        RenderTextureVK(Graphics& gfx, int w, int h, Format fmt);
        ~RenderTextureVK();

        void associateMultiRTResources(u64 id);

        const RenderInfo& getRenderInfo() const;

        vk::Format getFormat() const;
        const vk::Rect2D& getScissor() const;

        vk::ImageView getDepthView() const;

    private:
        GraphicsVK& graphics;
        vk::Format format;

        RenderInfo renderInfo;

        RawWrapper<TextureVK>::View depth;

        vk::Rect2D scissor;

        // This accumulates entries if a render texture is used together with lots of others that get quickly created/deleted.
        // Caused by the fact that resource deletion doesn't propagate to this field (semantically it doesn't have to).
        // Should realistically never be an issue.
        std::vector<u64> associatedMultiRTResources;
};

}

#endif // PGEINTERNAL_RENDERTEXTURE_VK_H_INCLUDED

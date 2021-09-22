#ifndef PGEINTERNAL_TEXTURE_DX11_H_INCLUDED
#define PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

#include <PGE/Graphics/Texture.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include "../../ResourceManagement/DX11.h"

namespace PGE {

class GraphicsDX11;
class TextureDX11 : public Texture {
    public:
        // Render target.
        TextureDX11(Graphics& gfx, int w, int h, Format fmt);
        // Loaded texture.
        TextureDX11(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps);
        // Loaded, compressed texture.
        TextureDX11(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt);

        void useTexture(int index);

        ID3D11RenderTargetView* getRtv() const;
        ID3D11DepthStencilView* getZBufferView() const;
        void* getNative() const override;

    private:
        GraphicsDX11& graphics;

        D3D11Texture2D::View dxTexture;
        D3D11ShaderResourceView::View dxShaderResourceView;

        D3D11RenderTargetView::View dxRtv;
        D3D11Texture2D::View dxZBufferTexture;
        D3D11DepthStencilView::View dxZBufferView;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

#ifndef PGEINTERNAL_TEXTURE_DX11_H_INCLUDED
#define PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <Texture/Texture.h>
#include <Threading/ThreadManager.h>

#include "../Graphics/GraphicsDX11.h"

namespace PGE {

class TextureDX11 : public Texture {
    public:
        TextureDX11(Graphics* gfx, int width, int height, bool renderTarget, const void* buffer, FORMAT fmt);
        TextureDX11(Graphics* gfx, const FilePath& fn, ThreadManager* threadManager);
        TextureDX11(Graphics* gfx, uint8_t* fiBuffer, int w, int h, int rw, int rh, const FilePath& fn);

        void useTexture(int index);

        virtual Texture* copy() const override;

        ID3D11RenderTargetView* getRtv() const;
        ID3D11DepthStencilView* getZBufferView() const;
        virtual void* getNative() const;

    private:
        D3D11_TEXTURE2D_DESC dxTextureDesc;
        D3D11Texture2D::Ref dxTexture;
        D3D11_SHADER_RESOURCE_VIEW_DESC dxShaderResourceViewDesc;
        D3D11ShaderResourceView::Ref dxShaderResourceView;

        D3D11RenderTargetView::Ref dxRtv;
        D3D11Texture2D::Ref dxZBufferTexture;
        D3D11DepthStencilView::Ref dxZBufferView;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

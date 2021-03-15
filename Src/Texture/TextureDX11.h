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
        TextureDX11(Graphics* gfx, uint8_t* fiBuffer, int w, int h, int rw, int rh, const FilePath& fn = FilePath::fromStr(""));

        void useTexture(int index);

        virtual Texture* copy() const override;

        D3D11RenderTargetViewRef getRtv() const;
        D3D11DepthStencilViewRef getZBufferView() const;
        virtual void* getNative() const;

    private:
        D3D11_TEXTURE2D_DESC dxTextureDesc;
        D3D11Texture2DRef dxTexture;
        D3D11_SHADER_RESOURCE_VIEW_DESC dxShaderResourceViewDesc;
        D3D11ShaderResourceViewRef dxShaderResourceView;

        D3D11RenderTargetViewRef dxRtv;
        D3D11Texture2DRef dxZBufferTexture;
        D3D11DepthStencilViewRef dxZBufferView;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

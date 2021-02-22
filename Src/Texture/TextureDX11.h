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

        ID3D11RenderTargetView* getRtv() const;
        ID3D11DepthStencilView* getZBufferView() const;
        virtual void* getNative() const;

    private:
        D3D11_TEXTURE2D_DESC dxTextureDesc;
        SmartRef<ID3D11Texture2D*> dxTexture;
        D3D11_SHADER_RESOURCE_VIEW_DESC dxShaderResourceViewDesc;
        SmartRef<ID3D11ShaderResourceView*> dxShaderResourceView;

        SmartRef<ID3D11RenderTargetView*> dxRtv;
        SmartRef<ID3D11Texture2D*> dxZBufferTexture;
        SmartRef<ID3D11DepthStencilView*> dxZBufferView;

        SmartOrderedDestructor destructor = 6;
};

}

#endif // PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

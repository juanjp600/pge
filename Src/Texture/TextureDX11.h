#ifndef PGEINTERNAL_TEXTURE_DX11_H_INCLUDED
#define PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <Windows.h>
#include <Texture/Texture.h>
#include <Threading/ThreadManager.h>

namespace PGE {

class TextureDX11 : public Texture {
    public:
        TextureDX11(Graphics* gfx, int width, int height, bool renderTarget, const void* buffer, FORMAT fmt);
        TextureDX11(Graphics* gfx, const FilePath& fn);
        TextureDX11(Graphics* gfx, const FilePath& fn, ThreadManager* threadManager);
        ~TextureDX11();

        void useTexture(int index);

        virtual bool isRenderTarget() const;

        ID3D11RenderTargetView* getRtv() const;
        ID3D11DepthStencilView* getZBufferView() const;
        virtual void* getNative() const;

    private:
        TextureDX11(){};

        void cleanup() override;
        void throwException(String func, String details) override;

        Graphics* graphics;

        D3D11_TEXTURE2D_DESC dxTextureDesc;
        D3D11_SUBRESOURCE_DATA dxTextureData;
        ID3D11Texture2D* dxTexture;
        D3D11_SHADER_RESOURCE_VIEW_DESC dxShaderResourceViewDesc;
        ID3D11ShaderResourceView* dxShaderResourceView;

        bool isRT;
        ID3D11RenderTargetView* dxRtv;
        ID3D11Texture2D* dxZBufferTexture;
        ID3D11DepthStencilView* dxZBufferView;
};

}

#endif // PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

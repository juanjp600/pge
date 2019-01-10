#ifndef PGEINTERNAL_TEXTURE_DX11_H_INCLUDED
#define PGEINTERNAL_TEXTURE_DX11_H_INCLUDED

#include <Texture/Texture.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

namespace PGE {

class TextureDX11 : public Texture {
    public:
        TextureDX11(Graphics* gfx,int width,int height,bool renderTarget=false,const void* buffer=nullptr);
        TextureDX11(Graphics* gfx,const String& fn);
        ~TextureDX11();

        void useTexture();

        virtual bool isRenderTarget() const;
    private:
        TextureDX11(){};

        Graphics* graphics;

        D3D11_TEXTURE2D_DESC dxTextureDesc;
        D3D11_SUBRESOURCE_DATA dxTextureData;
        ID3D11Texture2D* dxTexture;
        D3D11_SHADER_RESOURCE_VIEW_DESC dxShaderResourceViewDesc;
        ID3D11ShaderResourceView* dxShaderResourceView;

        bool isRT;
};

}

#endif

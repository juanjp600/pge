#include "TextureDX11.h"
#include "TextureInternal.h"
#include <Window/Window.h>
#include "../Window/WindowDX11.h"

#include <stdlib.h>
#include <inttypes.h>

using namespace PGE;

Texture* Texture::load(Graphics* gfx,String filename) {
    return new TextureDX11(gfx,filename);
}

Texture* Texture::create(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer) {
    return new TextureDX11(gfx,w,h,renderTarget,buffer);
}

TextureDX11::TextureDX11(Graphics* gfx,int w,int h,bool renderTarget,const void* buffer) {
    graphics = gfx;
    ID3D11Device* dxDevice = ((WindowDX11*)graphics->getWindow())->getDxDevice();
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

    filename = "<n/a>";

    void* newBuffer = nullptr;

    if (renderTarget) {
        name = "RenderTarget";
        buffer = nullptr;
    } else {
        name = "Static";
        if (buffer==nullptr) {
            name = String(name,"_Blank");
            newBuffer = new uint8_t[width*height*4];
            memset(newBuffer,0,width*height*4*sizeof(uint8_t));
            buffer = newBuffer;
        } else {
            name = String(name,"_Buffer");
        }
    }

    width = w; height = h;
    realWidth = width; realHeight = height;

    ZeroMemory( &dxTextureDesc,sizeof(D3D11_TEXTURE2D_DESC) );
    dxTextureDesc.Width = (UINT)realWidth;
    dxTextureDesc.Height = (UINT)realHeight;
    dxTextureDesc.MipLevels = 0;
    dxTextureDesc.ArraySize = 1;
    dxTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxTextureDesc.SampleDesc.Count = 1;
    dxTextureDesc.SampleDesc.Quality = 0;
    dxTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    dxTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    dxTextureDesc.CPUAccessFlags = 0;

    HRESULT hr = 0;

    hr = dxDevice->CreateTexture2D( &dxTextureDesc,NULL,&dxTexture );
    if (FAILED(hr)) {
        SDL_Log("1. %d %d %d\n",realWidth,realHeight,hr);
    }
    if (buffer != nullptr) { dxContext->UpdateSubresource(dxTexture,0,NULL,buffer,realWidth*4,0); }

    ZeroMemory( &dxShaderResourceViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
    dxShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    dxShaderResourceViewDesc.Texture2D.MipLevels = 1;
    hr = dxDevice->CreateShaderResourceView(dxTexture,&dxShaderResourceViewDesc,&dxShaderResourceView);
    if (FAILED(hr)) {
        SDL_Log("2. %d\n",hr);
    }

    isRT = renderTarget;

    if (isRT) {
        dxDevice->CreateRenderTargetView( dxTexture, NULL, &dxRtv );

        // Create depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth;
        ZeroMemory(&descDepth, sizeof(descDepth));
        descDepth.Width = realWidth;
        descDepth.Height = realHeight;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;
        hr = dxDevice->CreateTexture2D(&descDepth, NULL, &dxZBufferTexture);

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        ZeroMemory(&descDSV, sizeof(descDSV));
        descDSV.Format = descDepth.Format;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        hr = dxDevice->CreateDepthStencilView(dxZBufferTexture, &descDSV, &dxZBufferView);
    }

    if (newBuffer!=nullptr) { delete[] newBuffer; }
}

TextureDX11::TextureDX11(Graphics* gfx,const String& fn) {
    graphics = gfx;
    ID3D11Device* dxDevice = ((WindowDX11*)graphics->getWindow())->getDxDevice();
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

    filename = fn;
    name = fn;

    FIBITMAP* fiBuffer = loadFIBuffer(filename,width,height,realWidth,realHeight,opaque);

    ZeroMemory( &dxTextureDesc,sizeof(D3D11_TEXTURE2D_DESC) );
    dxTextureDesc.Width = (UINT)realWidth;
    dxTextureDesc.Height = (UINT)realHeight;
    dxTextureDesc.MipLevels = 0;
    dxTextureDesc.ArraySize = 1;
    dxTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxTextureDesc.SampleDesc.Count = 1;
    dxTextureDesc.SampleDesc.Quality = 0;
    dxTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    dxTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    dxTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    dxTextureDesc.CPUAccessFlags = 0;

    HRESULT hr = 0;

    hr = dxDevice->CreateTexture2D( &dxTextureDesc,NULL,&dxTexture );
    if (FAILED(hr)) {
        SDL_Log("1. %d %d %d\n",realWidth,realHeight,hr);
    }
    dxContext->UpdateSubresource(dxTexture,0,NULL,FreeImage_GetBits(fiBuffer),realWidth*4,0);

    ZeroMemory( &dxShaderResourceViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
    dxShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    dxShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    dxShaderResourceViewDesc.Texture2D.MipLevels = -1;
    hr = dxDevice->CreateShaderResourceView(dxTexture,&dxShaderResourceViewDesc,&dxShaderResourceView);
    if (FAILED(hr)) {
        SDL_Log("2. %d\n",hr);
    }

    dxContext->GenerateMips(dxShaderResourceView);

    isRT = false;

    FreeImage_Unload(fiBuffer);
}

TextureDX11::~TextureDX11() {
    dxShaderResourceView->Release();
    dxTexture->Release();
}

void TextureDX11::useTexture(int index) {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();
    dxContext->PSSetShaderResources(index,1,&dxShaderResourceView);
}

bool TextureDX11::isRenderTarget() const {
    return isRT;
}

ID3D11RenderTargetView* TextureDX11::getRtv() const {
    return dxRtv;
}

ID3D11DepthStencilView* TextureDX11::getZBufferView() const {
    return dxZBufferView;
}

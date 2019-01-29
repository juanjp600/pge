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
    dxTextureDesc.MipLevels = (UINT)1;
    dxTextureDesc.ArraySize = 1;
    dxTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxTextureDesc.SampleDesc.Count = 1;
    dxTextureDesc.SampleDesc.Quality = 0;
    dxTextureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    dxTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    dxTextureDesc.CPUAccessFlags = 0;

    ZeroMemory( &dxTextureData,sizeof(D3D11_SUBRESOURCE_DATA) );
    dxTextureData.pSysMem = buffer;
    dxTextureData.SysMemPitch = realWidth*4;
    dxTextureData.SysMemSlicePitch = realWidth*realHeight*4;

    HRESULT hr = 0;

    hr = dxDevice->CreateTexture2D( &dxTextureDesc,&dxTextureData,&dxTexture );
    if (FAILED(hr)) {
        SDL_Log("1. %d %d %d\n",realWidth,realHeight,hr);
    }

    ZeroMemory( &dxShaderResourceViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
    dxShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    dxShaderResourceViewDesc.Texture2D.MipLevels = 1;
    hr = dxDevice->CreateShaderResourceView(dxTexture,&dxShaderResourceViewDesc,&dxShaderResourceView);
    if (FAILED(hr)) {
        SDL_Log("2. %d\n",hr);
    }

    isRT = renderTarget;
    /*if (isRT) {
    glGenFramebuffers(1,&glFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,glFramebuffer);
    TODO: FINISH THIS
    }*/

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

    /*ZeroMemory( &dxTextureData,sizeof(D3D11_SUBRESOURCE_DATA) );
    dxTextureData.pSysMem = FreeImage_GetBits(fiBuffer);
    dxTextureData.SysMemPitch = realWidth*4;
    dxTextureData.SysMemSlicePitch = realWidth*realHeight*4;*/

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
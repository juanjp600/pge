#include "TextureDX11.h"
#include "TextureInternal.h"
#include <Exception/Exception.h>
#include <Threading/ThreadManager.h>
#include "../Graphics/GraphicsDX11.h"

using namespace PGE;

TextureDX11::TextureDX11(Graphics* gfx,int w,int h,bool renderTarget,const void* buffer,FORMAT fmt) : resourceManager(renderTarget ? 2 + 3 : 2) {
    isRT = renderTarget;

    graphics = gfx;
    ID3D11Device* dxDevice = ((GraphicsDX11*)graphics)->getDxDevice();
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    void* newBuffer = nullptr;

    format = fmt;

    width = w; height = h;
    realWidth = width; realHeight = height;

    if (renderTarget) {
        name = "RenderTarget";
        buffer = nullptr;
    } else {
        name = "Static";
        if (buffer==nullptr) {
            name = String(name, "_Blank");
            newBuffer = new uint8_t[(size_t)realWidth*(size_t)realHeight*4];
            memset(newBuffer, 0, (size_t)realWidth*(size_t)realHeight*4*sizeof(uint8_t));
            buffer = newBuffer;
        } else {
            name = String(name, "_Buffer");
        }
    }

    DXGI_FORMAT dxFormat;
    switch (format) {
        case FORMAT::RGBA32: {
            dxFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        } break;
        case FORMAT::R32F: {
            dxFormat = DXGI_FORMAT_R32_FLOAT;
        } break;
        default: {
            dxFormat = DXGI_FORMAT_UNKNOWN;
        }
    }

    ZeroMemory(&dxTextureDesc,sizeof(D3D11_TEXTURE2D_DESC));
    dxTextureDesc.Width = (UINT)realWidth;
    dxTextureDesc.Height = (UINT)realHeight;
    dxTextureDesc.MipLevels = 0;
    dxTextureDesc.ArraySize = 1;
    dxTextureDesc.Format = dxFormat;
    dxTextureDesc.SampleDesc.Count = 1;
    dxTextureDesc.SampleDesc.Quality = 0;
    dxTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    dxTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    dxTextureDesc.CPUAccessFlags = 0;

    dxTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, dxTextureDesc);
    if (buffer != nullptr) { dxContext->UpdateSubresource(dxTexture,0,NULL,buffer,realWidth*4,0); }

    ZeroMemory( &dxShaderResourceViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
    dxShaderResourceViewDesc.Format = dxFormat;
    dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    dxShaderResourceViewDesc.Texture2D.MipLevels = 1;
    dxShaderResourceView = resourceManager.addNewResource<D3D11ShaderResourceView>(dxDevice, dxTexture, dxShaderResourceViewDesc);

    if (isRT) {
        dxRtv = resourceManager.addNewResource<D3D11RenderTargetView>(dxDevice, dxTexture);

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
        dxZBufferTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, descDepth);

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        ZeroMemory(&descDSV, sizeof(descDSV));
        descDSV.Format = descDepth.Format;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        dxZBufferView = resourceManager.addNewResource<D3D11DepthStencilView>(dxDevice, dxZBufferTexture, descDSV);
    }

    if (newBuffer!=nullptr) { delete[] newBuffer; }
}

TextureDX11::TextureDX11(Graphics* gfx, uint8_t* fiBuffer, int w, int h, int rw, int rh, const FilePath& fn) : resourceManager(2) {
    graphics = gfx;
    width = w;
    height = h;
    realWidth = rw;
    realHeight = rh;
    filename = fn;
    name = fn.str();
    
    ID3D11Device* dxDevice = ((GraphicsDX11*)graphics)->getDxDevice();
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    format = FORMAT::RGBA32;

    ZeroMemory(&dxTextureDesc,sizeof(D3D11_TEXTURE2D_DESC));
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

    HRESULT hResult = 0;

    dxTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, dxTextureDesc);
    dxContext->UpdateSubresource(dxTexture,0,NULL,fiBuffer,realWidth*4,0);

    ZeroMemory( &dxShaderResourceViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
    dxShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    dxShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    dxShaderResourceViewDesc.Texture2D.MipLevels = -1;
    dxShaderResourceView = resourceManager.addNewResource<D3D11ShaderResourceView>(dxDevice, dxTexture, dxShaderResourceViewDesc);

    dxContext->GenerateMips(dxShaderResourceView);

    isRT = false;
}

TextureDX11::TextureDX11(Graphics* gfx,const FilePath& fn,ThreadManager* threadManager) : resourceManager(0) {
    __ASSERT(false, "Refactor me!"); // TODO
}

void TextureDX11::useTexture(int index) {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    ID3D11ShaderResourceView* srvArr[] { dxShaderResourceView };
    dxContext->PSSetShaderResources(index,1,srvArr);
}

Texture* TextureDX11::copy() const {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    TextureDX11* copy = new TextureDX11(graphics, getWidth(), getHeight(), false, nullptr, format);
    copy->name = String(name, "_Copy");

    dxContext->CopyResource(copy->dxTexture, dxTexture);

    // TODO: Check MipLevels, might have to regenerate dxShaderResourceView.

    return copy;
}

ID3D11RenderTargetView* TextureDX11::getRtv() const {
    return dxRtv;
}

ID3D11DepthStencilView* TextureDX11::getZBufferView() const {
    return dxZBufferView;
}

void* TextureDX11::getNative() const {
    return dxTexture;
}

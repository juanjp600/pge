#include "TextureDX11.h"
#include <Exception/Exception.h>
#include <Threading/ThreadManager.h>
#include "../Graphics/GraphicsDX11.h"

using namespace PGE;

static DXGI_FORMAT getDXFormat(Texture::FORMAT format) {
    switch (format) {
        case Texture::FORMAT::RGBA32: {
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        } break;
        case Texture::FORMAT::R32F: {
            return DXGI_FORMAT_R32_FLOAT;
        } break;
        default: {
            return DXGI_FORMAT_UNKNOWN;
        }
    }
}

TextureDX11::TextureDX11(Graphics* gfx, int w, int h, FORMAT fmt) : Texture(gfx, width, height, true, fmt), resourceManager(5) {
    ID3D11Device* dxDevice = ((GraphicsDX11*)gfx)->getDxDevice();

    DXGI_FORMAT dxFormat = getDXFormat(fmt);

    dxTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, D3D11Texture2D::TYPE::RENDER_TARGET, w, h, dxFormat);

    dxShaderResourceView = resourceManager.addNewResource<D3D11ShaderResourceView>(dxDevice, dxTexture, dxFormat, true);

    dxRtv = resourceManager.addNewResource<D3D11RenderTargetView>(dxDevice, dxTexture);

    dxZBufferTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, D3D11Texture2D::TYPE::DEPTH_STENCIL, w, h, DXGI_FORMAT_D24_UNORM_S8_UINT);

    dxZBufferView = resourceManager.addNewResource<D3D11DepthStencilView>(dxDevice, dxZBufferTexture, DXGI_FORMAT_D24_UNORM_S8_UINT);
}

TextureDX11::TextureDX11(Graphics* gfx, int w, int h, byte* buffer, FORMAT fmt) : Texture(gfx, width, height, false, fmt), resourceManager(2) {
    ID3D11Device* dxDevice = ((GraphicsDX11*)gfx)->getDxDevice();
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)gfx)->getDxContext();

    DXGI_FORMAT dxFormat = getDXFormat(fmt);

    dxTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, D3D11Texture2D::TYPE::NORMAL, w, h, dxFormat);
    dxContext->UpdateSubresource(dxTexture, 0, NULL, buffer, w * 4, 0);

    dxShaderResourceView = resourceManager.addNewResource<D3D11ShaderResourceView>(dxDevice, dxTexture, dxFormat, false);
    dxContext->GenerateMips(dxShaderResourceView);
}

void TextureDX11::useTexture(int index) {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    ID3D11ShaderResourceView* srvArr[] { dxShaderResourceView };
    dxContext->PSSetShaderResources(index,1,srvArr);
}

// TODO: Redo this.
Texture* TextureDX11::copy() const {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    //TextureDX11* copy = new TextureDX11(graphics, getWidth(), getHeight(), false, nullptr, format);

    //dxContext->CopyResource(copy->dxTexture, dxTexture);

    // TODO: Check MipLevels, might have to regenerate dxShaderResourceView.

    return nullptr;
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

#include "../GraphicsDX11.h"

#include <PGE/Types/Range.h>

using namespace PGE;

static DXGI_FORMAT getDXFormat(Texture::Format fmt) {
    switch (fmt) {
        using enum Texture::Format;
        case RGBA64: { return DXGI_FORMAT_R16G16B16A16_UNORM; }
        case RGBA32: { return DXGI_FORMAT_R8G8B8A8_UNORM; }
        case R32F: { return DXGI_FORMAT_R32_FLOAT; }
        case R8: { return DXGI_FORMAT_R8_UNORM; }
        default: { throw PGE_CREATE_EX("Invalid format"); }
    }
}

static DXGI_FORMAT getDXFormat(Texture::CompressedFormat fmt) {
    switch (fmt) {
        using enum Texture::CompressedFormat;
        case BC1: { return DXGI_FORMAT_BC1_UNORM; }
        case BC2: { return DXGI_FORMAT_BC2_UNORM; }
        case BC3: { return DXGI_FORMAT_BC3_UNORM; }
        case BC4: { return DXGI_FORMAT_BC4_UNORM; }
        case BC5: { return DXGI_FORMAT_BC5_UNORM; }
        case BC6: { return DXGI_FORMAT_BC6H_SF16; }
        case BC7: { return DXGI_FORMAT_BC7_UNORM; }
        default: { throw PGE_CREATE_EX("Invalid compressed format"); }
    }
}

static int getBitsPerBlockOnLine(Texture::CompressedFormat fmt) {
    switch (fmt) {
        using enum Texture::CompressedFormat;
        case BC1:
        case BC4: {
            return 2;
        }
        case BC2:
        case BC3:
        case BC5:
        case BC6:
        case BC7: {
            return 4;
        }
        default: {
            throw PGE_CREATE_EX("Invalid compressed format");
        }
    }
}

TextureDX11::TextureDX11(Graphics& gfx, int w, int h, Format fmt) : Texture(w, h, true, fmt), graphics((GraphicsDX11&)gfx) {
    ID3D11Device* dxDevice = graphics.getDxDevice();

    DXGI_FORMAT dxFormat = getDXFormat(fmt);

    dxTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, D3D11Texture2D::Type::RENDER_TARGET, w, h, dxFormat);

    dxShaderResourceView = resourceManager.addNewResource<D3D11ShaderResourceView>(dxDevice, dxTexture, dxFormat);

    dxRtv = resourceManager.addNewResource<D3D11RenderTargetView>(dxDevice, dxTexture);

    dxZBufferTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, D3D11Texture2D::Type::DEPTH_STENCIL, w, h, DXGI_FORMAT_D24_UNORM_S8_UINT);

    dxZBufferView = resourceManager.addNewResource<D3D11DepthStencilView>(dxDevice, dxZBufferTexture);
}

TextureDX11::TextureDX11(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps) : Texture(w, h, false, fmt), graphics((GraphicsDX11&)gfx) {
    ID3D11Device* dxDevice = graphics.getDxDevice();
    ID3D11DeviceContext* dxContext = graphics.getDxContext();

    DXGI_FORMAT dxFormat = getDXFormat(fmt);

    dxTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, mipmaps ? D3D11Texture2D::Type::NORMAL : D3D11Texture2D::Type::NO_MIPMAPS, w, h, dxFormat);
    dxContext->UpdateSubresource(dxTexture, 0, NULL, buffer, w * getBytesPerPixel(fmt), 0);

    dxShaderResourceView = resourceManager.addNewResource<D3D11ShaderResourceView>(dxDevice, dxTexture, dxFormat);
    dxContext->GenerateMips(dxShaderResourceView);
}

TextureDX11::TextureDX11(Graphics& gfx, const std::vector<Texture::Mipmap>& mipmaps, CompressedFormat fmt) : Texture(mipmaps[0].width, mipmaps[0].height, false, fmt), graphics((GraphicsDX11&)gfx) {
    ID3D11Device* dxDevice = graphics.getDxDevice();
    ID3D11DeviceContext* dxContext = graphics.getDxContext();

    DXGI_FORMAT dxFormat = getDXFormat(fmt);

    dxTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice,
        mipmaps.size() == 1 ? D3D11Texture2D::Type::COMPRESSED_NO_MIPMAPS : D3D11Texture2D::Type::COMPRESSED,
        mipmaps[0].width, mipmaps[0].height, dxFormat);
    for (size_t i : Range(mipmaps.size())) {
        dxContext->UpdateSubresource(dxTexture, D3D11CalcSubresource(i, 0, (UINT)mipmaps.size()), NULL, mipmaps[i].buffer, mipmaps[i].width * getBitsPerBlockOnLine(fmt), 0);
    }

    dxShaderResourceView = resourceManager.addNewResource<D3D11ShaderResourceView>(dxDevice, dxTexture, dxFormat);
}

void TextureDX11::useTexture(int index) {
    ID3D11DeviceContext* dxContext = graphics.getDxContext();
    dxContext->PSSetShaderResources(index, 1, &dxShaderResourceView);
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

#include "TextureDX11.h"
#include "TextureInternal.h"
#include <Window/Window.h>
#include "../Window/WindowDX11.h"
#include <Exception/Exception.h>
#include <Threading/ThreadManager.h>

#include <stdlib.h>
#include <inttypes.h>

using namespace PGE;

Texture* Texture::load(Graphics* gfx, const void* buffer, int size) {
    return new TextureDX11(gfx, buffer, size);
}

Texture* Texture::load(Graphics* gfx, const FilePath& filename) {
    return new TextureDX11(gfx,filename);
}

Texture* Texture::load(Graphics* gfx, const FilePath& filename,ThreadManager* threadManager) {
    return new TextureDX11(gfx,filename,threadManager);
}

Texture* Texture::create(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer,FORMAT fmt) {
    return new TextureDX11(gfx,w,h,renderTarget,buffer,fmt);
}

TextureDX11::TextureDX11(Graphics* gfx,int w,int h,bool renderTarget,const void* buffer,FORMAT fmt) {
    dxShaderResourceView = nullptr;
    dxTexture = nullptr;
    dxRtv = nullptr;
    dxZBufferView = nullptr;
    dxZBufferTexture = nullptr;
    isRT = false;

    graphics = gfx;
    ID3D11Device* dxDevice = ((WindowDX11*)graphics->getWindow())->getDxDevice();
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

    // filename = "<n/a>";

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

    HRESULT hResult = 0;

    hResult = dxDevice->CreateTexture2D(&dxTextureDesc,NULL,&dxTexture);
    if (FAILED(hResult)) {
        throwException("TextureDX11(w,h,rt)","Failed to create D3D11 texture ("+String::fromInt(realWidth)+","+String::fromInt(realHeight)+"; HRESULT "+String::fromInt(hResult)+")");
    }
    if (buffer != nullptr) { dxContext->UpdateSubresource(dxTexture,0,NULL,buffer,realWidth*4,0); }

    ZeroMemory( &dxShaderResourceViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
    dxShaderResourceViewDesc.Format = dxFormat;
    dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    dxShaderResourceViewDesc.Texture2D.MipLevels = 1;
    hResult = dxDevice->CreateShaderResourceView(dxTexture,&dxShaderResourceViewDesc,&dxShaderResourceView);
    if (FAILED(hResult)) {
        throwException("TextureDX11(w,h,rt)", "Failed to create shader resource view ("+String::fromInt(realWidth)+","+String::fromInt(realHeight)+"; HRESULT "+String::fromInt(hResult) + ")");
    }

    isRT = renderTarget;

    if (isRT) {
        dxDevice->CreateRenderTargetView(dxTexture, NULL, &dxRtv);

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
        hResult = dxDevice->CreateTexture2D(&descDepth, NULL, &dxZBufferTexture);
        if (FAILED(hResult)) {
            throwException("TextureDX11(w,h,rt)", "Failed to create ZBuffer texture ("+String::fromInt(realWidth)+","+String::fromInt(realHeight)+"; HRESULT "+String::fromInt(hResult)+")");
        }

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        ZeroMemory(&descDSV, sizeof(descDSV));
        descDSV.Format = descDepth.Format;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        hResult = dxDevice->CreateDepthStencilView(dxZBufferTexture, &descDSV, &dxZBufferView);
        if (FAILED(hResult)) {
            throwException("TextureDX11(w,h,rt)", "Failed to create depth stencil view ("+String::fromInt(realWidth)+","+String::fromInt(realHeight)+"; HRESULT "+String::fromInt(hResult)+")");
        }
    }

    if (newBuffer!=nullptr) { delete[] newBuffer; }
}

TextureDX11::TextureDX11(Graphics* gfx, const void* buffer, int size) {
    graphics = gfx;
    BYTE* fiBuffer = nullptr;
    try {
        fiBuffer = loadFIBufferFromMemory(buffer, size, width, height, realWidth, realHeight);
    } catch (Exception& e) {
        cleanup();
        throw e;
    } catch (std::exception& e) {
        cleanup();
        throw e;
    }
    initLoad(fiBuffer);
}

TextureDX11::TextureDX11(Graphics* gfx, const FilePath& fn) {
    graphics = gfx;
    filename = fn;
    name = fn.str();
    BYTE* fiBuffer = nullptr;
    try {
        fiBuffer = loadFIBufferFromFile(filename, width, height, realWidth, realHeight);
    } catch (Exception& e) {
        cleanup();
        throw e;
    } catch (std::exception& e) {
        cleanup();
        throw e;
    }
    initLoad(fiBuffer);
}

void TextureDX11::initLoad(BYTE* fiBuffer) {
    dxShaderResourceView = nullptr;
    dxTexture = nullptr;
    dxRtv = nullptr;
    dxZBufferView = nullptr;
    dxZBufferTexture = nullptr;
    isRT = false;
    
    ID3D11Device* dxDevice = ((WindowDX11*)graphics->getWindow())->getDxDevice();
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

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

    hResult = dxDevice->CreateTexture2D(&dxTextureDesc,NULL,&dxTexture);
    if (FAILED(hResult)) {
        throwException("TextureDX11(fn)", "Failed to create D3D11 texture (filename: "+filename.str()+", HRESULT "+String::fromInt(hResult)+")");
    }
    dxContext->UpdateSubresource(dxTexture,0,NULL,fiBuffer,realWidth*4,0);

    ZeroMemory( &dxShaderResourceViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
    dxShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    dxShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    dxShaderResourceViewDesc.Texture2D.MipLevels = -1;
    hResult = dxDevice->CreateShaderResourceView(dxTexture,&dxShaderResourceViewDesc,&dxShaderResourceView);
    if (FAILED(hResult)) {
        throwException("TextureDX11(fn)","Failed to create shader resource view (filename: "+filename.str()+", HRESULT "+String::fromInt(hResult)+")");
    }

    dxContext->GenerateMips(dxShaderResourceView);

    isRT = false;

    delete[] fiBuffer;
}

TextureDX11::TextureDX11(Graphics* gfx,const FilePath& fn,ThreadManager* threadManager) {
    dxShaderResourceView = nullptr;
    dxTexture = nullptr;
    dxRtv = nullptr;
    dxZBufferView = nullptr;
    dxZBufferTexture = nullptr;
    isRT = false;

    graphics = gfx;
    ID3D11Device* dxDevice = ((WindowDX11*)graphics->getWindow())->getDxDevice();
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

    filename = fn;
    name = fn.str();

    format = FORMAT::RGBA32;

    width = 512;
    height = 512;
    realWidth = 512;
    realHeight = 512;

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
    dxTextureDesc.CPUAccessFlags = 0;

    HRESULT hResult = 0;

    hResult = dxDevice->CreateTexture2D(&dxTextureDesc,NULL,&dxTexture);
    if (FAILED(hResult)) {
        throwException("TextureDX11(fn,threadMgr)", "Failed to create D3D11 texture (filename: "+filename.str()+", HRESULT "+String::fromInt(hResult)+")");
    }

    ZeroMemory( &dxShaderResourceViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
    dxShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    // TODO: Different from regular texture loading.
    dxShaderResourceViewDesc.Texture2D.MipLevels = 1;
    hResult = dxDevice->CreateShaderResourceView(dxTexture,&dxShaderResourceViewDesc,&dxShaderResourceView);
    if (FAILED(hResult)) {
        throwException("TextureDX11(fn,threadMgr)", "Failed to create shader resource view (filename: "+filename.str()+", HRESULT " + String::fromInt(hResult) + ")");
    }

    isRT = false;

    class TextureReassignRequest : public ThreadManager::MainThreadRequest {
        public:
            FilePath filename;
            D3D11_TEXTURE2D_DESC* dxTextureDesc;
            ID3D11Texture2D** dxTexture;
            D3D11_SHADER_RESOURCE_VIEW_DESC* dxShaderResourceViewDesc;
            ID3D11ShaderResourceView** dxShaderResourceView;
            ID3D11Device* dxDevice;
            ID3D11DeviceContext* dxContext;

            int realWidth; int realHeight;
            BYTE* buffer;

            void execute() {
                ID3D11Texture2D* tempTex = nullptr;
                ID3D11ShaderResourceView* tempResView = nullptr;
                D3D11_TEXTURE2D_DESC tempTexDesc;
                D3D11_SHADER_RESOURCE_VIEW_DESC tempResViewDesc;

                ZeroMemory(&tempTexDesc,sizeof(D3D11_TEXTURE2D_DESC));
                tempTexDesc.Width = (UINT)realWidth;
                tempTexDesc.Height = (UINT)realHeight;
                tempTexDesc.MipLevels = 0;
                tempTexDesc.ArraySize = 1;
                tempTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                tempTexDesc.SampleDesc.Count = 1;
                tempTexDesc.SampleDesc.Quality = 0;
                tempTexDesc.Usage = D3D11_USAGE_DEFAULT;
                tempTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
                tempTexDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
                tempTexDesc.CPUAccessFlags = 0;

                HRESULT hResult = 0;

                hResult = dxDevice->CreateTexture2D(&tempTexDesc,NULL,&tempTex);
                if (FAILED(hResult)) {
                    throw Exception("TextureReassignRequest (DX11)","Failed to create D3D11Texture (filename: "+filename.str()+", HRESULT "+String::fromInt(hResult)+")");
                }
                if (buffer != nullptr) { dxContext->UpdateSubresource(tempTex,0,NULL,buffer,realWidth*4,0); }

                ZeroMemory(&tempResViewDesc,sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
                tempResViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                tempResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                tempResViewDesc.Texture2D.MostDetailedMip = 0;
                tempResViewDesc.Texture2D.MipLevels = -1;
                hResult = dxDevice->CreateShaderResourceView(tempTex,&tempResViewDesc,&tempResView);
                if (FAILED(hResult)) {
                    tempTex->Release();
                    throw Exception("TextureReassignRequest (DX11)", "Failed to create shader resource view (filename: "+filename.str()+", HRESULT "+String::fromInt(hResult)+")");
                }

                dxContext->GenerateMips(tempResView);

                (*dxShaderResourceView)->Release();
                (*dxTexture)->Release();
                *dxShaderResourceView = tempResView;
                *dxTexture = tempTex;
                memcpy(dxShaderResourceViewDesc, &tempResViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
                memcpy(dxTextureDesc, &tempTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
            }
    } mainThreadRequest;

    mainThreadRequest.filename = filename;
    mainThreadRequest.dxTextureDesc = &dxTextureDesc;
    mainThreadRequest.dxTexture = &dxTexture;
    mainThreadRequest.dxShaderResourceViewDesc = &dxShaderResourceViewDesc;
    mainThreadRequest.dxShaderResourceView = &dxShaderResourceView;
    mainThreadRequest.dxDevice = dxDevice;
    mainThreadRequest.dxContext = dxContext;

    class TextureLoadRequest : public ThreadManager::NewThreadRequest {
        public:
            TextureReassignRequest mainThreadRequest;
            FilePath filename;
            int* width; int* height; int* realWidth; int* realHeight;
            void execute() {
                BYTE* fiBuffer = loadFIBufferFromFile(filename,*width,*height,*realWidth,*realHeight);

                mainThreadRequest.realWidth = *realWidth;
                mainThreadRequest.realHeight = *realHeight;
                mainThreadRequest.buffer = fiBuffer;
                requestExecutionOnMainThread(&mainThreadRequest);

                delete[] fiBuffer;

                markAsDone();
            }
    };

    TextureLoadRequest* textureLoadRequest = new TextureLoadRequest();
    textureLoadRequest->mainThreadRequest = mainThreadRequest;
    textureLoadRequest->filename = filename;
    textureLoadRequest->width = &width;
    textureLoadRequest->height = &height;
    textureLoadRequest->realWidth = &realWidth;
    textureLoadRequest->realHeight = &realHeight;

    threadManager->requestExecutionOnNewThread(textureLoadRequest);
}

TextureDX11::~TextureDX11() {
    cleanup();
}

void TextureDX11::throwException(String func, String details) {
    cleanup();
    throw Exception("TextureDX11::"+func,details);
}

void TextureDX11::cleanup() {
    if (dxShaderResourceView != nullptr) { dxShaderResourceView->Release(); }
    if (dxTexture != nullptr) { dxTexture->Release(); }
    if (isRT) {
        if (dxRtv != nullptr) { dxRtv->Release(); }
        if (dxZBufferView != nullptr) { dxZBufferView->Release(); }
        if (dxZBufferTexture != nullptr) { dxZBufferTexture->Release(); }
    }

    dxShaderResourceView = nullptr;
    dxTexture = nullptr;
    dxRtv = nullptr;
    dxZBufferView = nullptr;
    dxZBufferTexture = nullptr;
    isRT = false;
}

void TextureDX11::useTexture(int index) {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();
    dxContext->PSSetShaderResources(index,1,&dxShaderResourceView);
}

bool TextureDX11::isRenderTarget() const {
    return isRT;
}

Texture* TextureDX11::copy() const {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

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

#include "GraphicsDX11.h"

#include <SDL_syswm.h>
#include <stdlib.h>

#include "../Shader/ShaderDX11.h"
#include "../Mesh/MeshDX11.h"
#include "../Texture/TextureDX11.h"

using namespace PGE;

//REMINDER: https://code.msdn.microsoft.com/windowsdesktop/Direct3D-Tutorial-Win32-829979ef

GraphicsDX11::GraphicsDX11(String name,int w,int h,bool fs) : GraphicsInternal(name, w, h, fs, SDL_WINDOW_SHOWN), resourceManager(12) {
    HRESULT hResult = 0;
    int errorCode = 0;

    if (fullscreen) {
        SDL_SetWindowBordered(sdlWindow, SDL_bool::SDL_FALSE);
        SDL_Rect displayBounds;
        int displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
        if (displayIndex < 0) {
            throw Exception("GraphicsDX11", "Failed to determine display index: " + String(SDL_GetError()));
        }
        errorCode = SDL_GetDisplayBounds(displayIndex, &displayBounds);
        if (errorCode < 0) {
            throw Exception("GraphicsDX11", "Failed to get display bounds: " + String(SDL_GetError()));
        }
        if (displayBounds.w <= 0 || displayBounds.h <= 0) {
            throw Exception("GraphicsDX11", "Display bounds are invalid (" + String::fromInt(displayBounds.w) + ", " + String::fromInt(displayBounds.h) + ")");
        }
        SDL_SetWindowSize(sdlWindow, displayBounds.w, displayBounds.h);
        SDL_SetWindowPosition(sdlWindow, 0, 0);
    }

    dxgiFactory = DXGIFactory1::createRef(resourceManager);

    SDL_SysWMinfo sysWMinfo;
    SDL_VERSION(&sysWMinfo.version); //REMINDER: THIS LINE IS VERY IMPORTANT
    bool validInfo = SDL_GetWindowWMInfo(sdlWindow, &sysWMinfo);
    if (!validInfo) {
        throw Exception("GraphicsDX11", "Failed to initialize SDL version info: " + String(SDL_GetError()));
    }

    ZeroMemory(&dxSwapChainDesc, sizeof(dxSwapChainDesc));
    dxSwapChainDesc.BufferCount = 1;
    dxSwapChainDesc.BufferDesc.Width = w;
    dxSwapChainDesc.BufferDesc.Height = h;
    dxSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    dxSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    dxSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dxSwapChainDesc.OutputWindow = sysWMinfo.info.win.window;
    dxSwapChainDesc.SampleDesc.Count = 1;
    dxSwapChainDesc.SampleDesc.Quality = 0;
    dxSwapChainDesc.Windowed = TRUE;

    dxDevice = D3D11Device::createRef(resourceManager);
    dxContext = D3D11ImmediateContext::createRef(resourceManager, dxDevice);
    dxSwapChain = DXGISwapChain::createRef(resourceManager, dxgiFactory, dxDevice, dxSwapChainDesc);
    dxBackBufferRtv = D3D11BackBufferRtv::createRef(resourceManager, dxDevice, dxSwapChain);

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    dxZBufferTexture = D3D11Texture2D::createRef(resourceManager, dxDevice, descDepth);

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    dxZBufferView = D3D11DepthStencilView::createRef(resourceManager, dxDevice, dxZBufferTexture, descDSV);

    dxContext->OMSetRenderTargets(1, &dxBackBufferRtv, dxZBufferView);

    ZeroMemory(&dxRasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));
    dxRasterizerStateDesc.AntialiasedLineEnable = false;
    dxRasterizerStateDesc.CullMode = D3D11_CULL_BACK;
    dxRasterizerStateDesc.DepthClipEnable = true;
    dxRasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
    dxRasterizerStateDesc.ScissorEnable = false;
    dxRasterizerStateDesc.MultisampleEnable = false;
    dxRasterizerStateDesc.FrontCounterClockwise = true;
    dxRasterizerState = D3D11RasterizerState::createRef(resourceManager, dxDevice, dxRasterizerStateDesc);
    
    dxContext->RSSetState(dxRasterizerState);

    ZeroMemory(&dxBlendStateDesc, sizeof(D3D11_BLEND_DESC));
    dxBlendStateDesc.RenderTarget[0].BlendEnable = true;
    dxBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    dxBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    dxBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    dxBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    dxBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    dxBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    dxBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    dxBlendState = D3D11BlendState::createRef(resourceManager, dxDevice, dxBlendStateDesc);

    dxContext->OMSetBlendState(dxBlendState, 0, 0xffffffff);

    dxDepthStencilState = ResourceReferenceVector<ID3D11DepthStencilState*>::withSize(3);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    //Stencil operations if pixel is front-facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    //Stencil operations if pixel is back-facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE] = D3D11DepthStencilState::createRef(resourceManager, dxDevice, depthStencilDesc);
    dxContext->OMSetDepthStencilState(dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE], 0);

    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_NOWRITE] = D3D11DepthStencilState::createRef(resourceManager, dxDevice, depthStencilDesc);

    depthStencilDesc.DepthEnable = FALSE;
    dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::DISABLED] = D3D11DepthStencilState::createRef(resourceManager, dxDevice, depthStencilDesc);

    setViewport(Rectanglei(0,0,w,h));
    currentRenderTargetViews.add(dxBackBufferRtv);
    currentDepthStencilView = dxZBufferView;

    depthTest = true;
}

void GraphicsDX11::swap() {
    dxSwapChain->Present(vsync ? 1 : 0, 0);
}

void GraphicsDX11::clear(Color color) {
    float clearColor[4] = {color.red,color.green,color.blue,color.alpha};
    for (int i = 0; i < (int)currentRenderTargetViews.size(); i++) {
        dxContext->ClearRenderTargetView( currentRenderTargetViews[i], clearColor );
    }
    dxContext->ClearDepthStencilView( currentDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.f, 0 );
}

void GraphicsDX11::setRenderTarget(Texture* renderTarget) {
    for (int i = 0; i < (int)currentRenderTargetViews.size(); i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    dxContext->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear(); currentRenderTargetViews.add(((TextureDX11*)renderTarget)->getRtv());
    currentDepthStencilView = ((TextureDX11*)renderTarget)->getZBufferView();
    dxContext->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::setRenderTargets(std::vector<Texture*> renderTargets) {
    for (int i = 0; i < (int)currentRenderTargetViews.size(); i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    dxContext->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear();
    TextureDX11* maxSizeTexture = (TextureDX11*)renderTargets[0];
    for (int i = 0; i < (int)renderTargets.size(); i++) {
        if (!renderTargets[i]->isRenderTarget()) {
            throw Exception("setRenderTargets","renderTargets["+String::fromInt(i)+"] is not a valid render target");
        }
        currentRenderTargetViews.add(((TextureDX11*)renderTargets[i])->getRtv());
        if (renderTargets[i]->getWidth()+renderTargets[i]->getHeight()>maxSizeTexture->getWidth()+maxSizeTexture->getHeight()) {
            maxSizeTexture = (TextureDX11*)renderTargets[i];
        }
    }
    for (int i = 0; i < (int)renderTargets.size(); i++) {
        if (renderTargets[i]->getWidth()>maxSizeTexture->getWidth() || renderTargets[i]->getHeight()>maxSizeTexture->getHeight()) {
            throw Exception("setRenderTargets",
                "Render target sizes are incompatible ("+String::fromInt(maxSizeTexture->getWidth())+","+String::fromInt(maxSizeTexture->getHeight())+" vs "+
                                                         String::fromInt(renderTargets[i]->getWidth())+","+String::fromInt(renderTargets[i]->getHeight())+")");
        }
    }
    currentDepthStencilView = maxSizeTexture->getZBufferView();
    dxContext->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::resetRenderTarget() {
    for (int i = 0; i < (int)currentRenderTargetViews.size(); i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    dxContext->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear(); currentRenderTargetViews.add(dxBackBufferRtv);
    currentDepthStencilView = dxZBufferView;
    dxContext->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::setViewport(Rectanglei vp) {
    if (vp != viewport) {
        viewport = vp;

        dxViewport.Width = (FLOAT)viewport.width();
        dxViewport.Height = (FLOAT)viewport.height();
        dxViewport.MinDepth = 0.0f;
        dxViewport.MaxDepth = 1.0f;
        dxViewport.TopLeftX = (FLOAT)viewport.topLeftCorner().x;
        dxViewport.TopLeftY = (FLOAT)viewport.topLeftCorner().y;
        dxContext->RSSetViewports( 1, &dxViewport );
    }
}

ID3D11Device* GraphicsDX11::getDxDevice() const {
    return dxDevice;
}

ID3D11DeviceContext* GraphicsDX11::getDxContext() const {
    return dxContext;
}

ID3D11RenderTargetView* GraphicsDX11::getBackBufferRtv() const {
    return dxBackBufferRtv;
}

ID3D11DepthStencilView* GraphicsDX11::getZBufferView() const {
    return dxZBufferView;
}

void GraphicsDX11::setZBufferState(GraphicsDX11::ZBUFFER_STATE_INDEX index) {
    dxContext->OMSetDepthStencilState(dxDepthStencilState[(int)index], 0);
}

__GFX_OBJ_DEF(DX11)

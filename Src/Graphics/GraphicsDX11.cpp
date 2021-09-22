#include "GraphicsDX11.h"

#include <stdlib.h>

using namespace PGE;

// REMINDER: https://code.msdn.microsoft.com/windowsdesktop/Direct3D-Tutorial-Win32-829979ef

GraphicsDX11::GraphicsDX11(const String& name, int w, int h, WindowMode wm, int x, int y)
    : GraphicsSpecialized("DirectX 11", name, w, h, wm, x, y, SDL_WINDOW_SHOWN) {
    if (windowMode == WindowMode::Fullscreen) {
        SDL_SetWindowBordered(getWindow(), SDL_bool::SDL_FALSE);
        SDL_Rect displayBounds;
        int displayIndex = SDL_GetWindowDisplayIndex(getWindow());
        PGE_ASSERT(displayIndex >= 0, "Failed to determine display index (SDLERROR: " + String(SDL_GetError()) + ")");
        int errorCode = SDL_GetDisplayBounds(displayIndex, &displayBounds);
        PGE_ASSERT(errorCode == 0, "Failed to get display bounds (SDLERROR: " + String(SDL_GetError()) + ")");
        PGE_ASSERT(displayBounds.w > 0 && displayBounds.h > 0, "Display bounds are invalid (" + String::from(displayBounds.w) + "x" + String::from(displayBounds.h) + ")");
        SDL_SetWindowSize(getWindow(), displayBounds.w, displayBounds.h);
        SDL_SetWindowPosition(getWindow(), 0, 0);
    }

    dxgiFactory = resourceManager.addNewResource<DXGIFactory1>();

    dxDevice = resourceManager.addNewResource<D3D11Device>();
    dxContext = resourceManager.addNewResource<D3D11ImmediateContext>(dxDevice);
    dxSwapChain = resourceManager.addNewResource<DXGISwapChain>(dxDevice, dxgiFactory, w, h, getWindow());
    dxBackBufferRtv = resourceManager.addNewResource<D3D11BackBufferRtv>(dxDevice, dxSwapChain);

    dxZBufferTexture = resourceManager.addNewResource<D3D11Texture2D>(dxDevice, D3D11Texture2D::Type::DEPTH_STENCIL, w, h, DXGI_FORMAT_D24_UNORM_S8_UINT);

    dxZBufferView = resourceManager.addNewResource<D3D11DepthStencilView>(dxDevice, dxZBufferTexture);
    dxContext->OMSetRenderTargets(1, &dxBackBufferRtv, dxZBufferView);

    cullingMode = Culling::NONE;
    setCulling(Culling::BACK);

    dxBlendState = resourceManager.addNewResource<D3D11BlendState>(dxDevice);

    dxContext->OMSetBlendState(dxBlendState, 0, 0xffffffff);

    dxDepthStencilState = std::vector<ID3D11DepthStencilState*>(3);

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

    dxDepthStencilState[(int)ZBufferStateIndex::ENABLED_WRITE] = resourceManager.addNewResource<D3D11DepthStencilState>(dxDevice, depthStencilDesc);
    dxContext->OMSetDepthStencilState(dxDepthStencilState[(int)ZBufferStateIndex::ENABLED_WRITE], 0);

    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dxDepthStencilState[(int)ZBufferStateIndex::ENABLED_NOWRITE] = resourceManager.addNewResource<D3D11DepthStencilState>(dxDevice, depthStencilDesc);

    depthStencilDesc.DepthEnable = FALSE;
    dxDepthStencilState[(int)ZBufferStateIndex::DISABLED] = resourceManager.addNewResource<D3D11DepthStencilState>(dxDevice, depthStencilDesc);

    setViewport(Rectanglei(0,0,w,h));
    currentRenderTargetViews.emplace_back(dxBackBufferRtv);
    currentDepthStencilView = dxZBufferView;

    depthTest = true;
}

void GraphicsDX11::swap() {
    dxSwapChain->Present(vsync ? 1 : 0, 0);
}

void GraphicsDX11::clear(const Color& color) {
    float clearColor[4] = {color.red,color.green,color.blue,color.alpha};
    for (int i = 0; i < (int)currentRenderTargetViews.size(); i++) {
        dxContext->ClearRenderTargetView( currentRenderTargetViews[i], clearColor );
    }
    dxContext->ClearDepthStencilView( currentDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.f, 0 );
}

void GraphicsDX11::setRenderTarget(Texture& renderTarget) {
    for (int i = 0; i < (int)currentRenderTargetViews.size(); i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    dxContext->OMSetRenderTargets( (UINT)currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear(); currentRenderTargetViews.emplace_back(((TextureDX11&)renderTarget).getRtv());
    currentDepthStencilView = ((TextureDX11&)renderTarget).getZBufferView();
    dxContext->OMSetRenderTargets( (UINT)currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::setRenderTargets(const ReferenceVector<Texture>& renderTargets) {
    for (int i = 0; i < (int)currentRenderTargetViews.size(); i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    dxContext->OMSetRenderTargets( (UINT)currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear();
    TextureDX11* maxSizeTexture = &(TextureDX11&)renderTargets[0];
    for (int i = 0; i < (int)renderTargets.size(); i++) {
        PGE_ASSERT(renderTargets[i].get().isRenderTarget(), "renderTargets[" + String::from(i) + "] is not a valid render target");
        currentRenderTargetViews.emplace_back(((TextureDX11&)renderTargets[i]).getRtv());
        if (renderTargets[i].get().getWidth()+renderTargets[i].get().getHeight()>maxSizeTexture->getWidth()+maxSizeTexture->getHeight()) {
            maxSizeTexture = &(TextureDX11&)renderTargets[i];
        }
    }
    for (int i = 0; i < (int)renderTargets.size(); i++) {
        PGE_ASSERT(renderTargets[i].get().getWidth() <= maxSizeTexture->getWidth() && renderTargets[i].get().getHeight() <= maxSizeTexture->getHeight(),
            "Render target sizes are incompatible (" + String::from(maxSizeTexture->getWidth()) + "x" + String::from(maxSizeTexture->getHeight()) + " vs " +
                                                       String::from(renderTargets[i].get().getWidth()) + "x" + String::from(renderTargets[i].get().getHeight()) + ")");
    }
    currentDepthStencilView = maxSizeTexture->getZBufferView();
    dxContext->OMSetRenderTargets( (UINT)currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::resetRenderTarget() {
    for (int i = 0; i < (int)currentRenderTargetViews.size(); i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    dxContext->OMSetRenderTargets( (UINT)currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear(); currentRenderTargetViews.emplace_back(dxBackBufferRtv);
    currentDepthStencilView = dxZBufferView;
    dxContext->OMSetRenderTargets( (UINT)currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::setViewport(const Rectanglei& vp) {
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

void GraphicsDX11::setCulling(Culling mode) {
    if (mode == cullingMode) { return; }

    if (dxRasterizerState.isHoldingResource()) {
        resourceManager.deleteResource(dxRasterizerState);
    }

    dxRasterizerState = resourceManager.addNewResource<D3D11RasterizerState>(dxDevice, mode);

    dxContext->RSSetState(dxRasterizerState);

    cullingMode = mode;
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

void GraphicsDX11::setZBufferState(GraphicsDX11::ZBufferStateIndex index) {
    dxContext->OMSetDepthStencilState(dxDepthStencilState[(int)index], 0);
}

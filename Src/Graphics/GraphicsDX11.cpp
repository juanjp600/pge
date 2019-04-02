#include "GraphicsDX11.h"
#include <Window/Window.h>
#include "../Window/WindowDX11.h"
#include "../Texture/TextureDX11.h"
#include "../Exception/Exception.h"

#include <stdlib.h>

using namespace PGE;

Graphics* Graphics::create(String name,int w,int h,bool fs) {
    return new GraphicsDX11(name,w,h,fs);
}

GraphicsDX11::GraphicsDX11(String name,int w,int h,bool fs) {
    try {
        window = nullptr;
        window = new WindowDX11(name,w,h,fs);
    } catch (Exception& e) {
        cleanup();
        throw e;
    } catch (std::exception& e) {
        cleanup();
        throw e;
    }

    ID3D11Device* dxDevice = ((WindowDX11*)window)->getDxDevice();
    ID3D11DeviceContext* dxContext = ((WindowDX11*)window)->getDxContext();

    setViewport(Rectanglei(0,0,w,h));
    currentRenderTargetViews.push_back(((WindowDX11*)window)->getBackBufferRtv());
    currentDepthStencilView = ((WindowDX11*)window)->getZBufferView();
}

GraphicsDX11::~GraphicsDX11() {
    cleanup();
}

void GraphicsDX11::cleanup() {
    if (window != nullptr) { delete window; }
    window = nullptr;
}

void GraphicsDX11::throwException(String func,String details) {
    cleanup();
    throw Exception("GraphicsDX11::"+func,details);
}

void GraphicsDX11::update() {
    Graphics::update();
}

void GraphicsDX11::clear(Color color) {
    float clearColor[4] = {color.red,color.green,color.blue,color.alpha};
    for (int i=0;i<currentRenderTargetViews.size();i++) {
        ((WindowDX11*)window)->getDxContext()->ClearRenderTargetView( currentRenderTargetViews[i], clearColor );
    }
    ((WindowDX11*)window)->getDxContext()->ClearDepthStencilView( currentDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.f, 0 );
}

void GraphicsDX11::setRenderTarget(Texture* renderTarget) {
    for (int i=0;i<currentRenderTargetViews.size();i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    ((WindowDX11*)window)->getDxContext()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear(); currentRenderTargetViews.push_back(((TextureDX11*)renderTarget)->getRtv());
    currentDepthStencilView = ((TextureDX11*)renderTarget)->getZBufferView();
    ((WindowDX11*)window)->getDxContext()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::setRenderTargets(std::vector<Texture*> renderTargets) {
    for (int i=0;i<currentRenderTargetViews.size();i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    ((WindowDX11*)window)->getDxContext()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear();
    TextureDX11* maxSizeTexture = (TextureDX11*)renderTargets[0];
    for (int i=0;i<renderTargets.size();i++) {
        if (!renderTargets[i]->isRenderTarget()) {
            throwException("setRenderTargets","renderTargets["+String(i)+"] is not a valid render target");
        }
        currentRenderTargetViews.push_back(((TextureDX11*)renderTargets[i])->getRtv());
        if (renderTargets[i]->getWidth()+renderTargets[i]->getHeight()>maxSizeTexture->getWidth()+maxSizeTexture->getHeight()) {
            maxSizeTexture = (TextureDX11*)renderTargets[i];
        }
    }
    for (int i=0;i<renderTargets.size();i++) {
        if (renderTargets[i]->getWidth()>maxSizeTexture->getWidth() || renderTargets[i]->getHeight()>maxSizeTexture->getHeight()) {
            throwException("setRenderTargets",
                "Render target sizes are incompatible ("+String(maxSizeTexture->getWidth())+","+String(maxSizeTexture->getHeight())+" vs "+
                                                         String(renderTargets[i]->getWidth())+","+String(renderTargets[i]->getHeight())+")");
        }
    }
    currentDepthStencilView = maxSizeTexture->getZBufferView();
    ((WindowDX11*)window)->getDxContext()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::resetRenderTarget() {
    for (int i=0;i<currentRenderTargetViews.size();i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    ((WindowDX11*)window)->getDxContext()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear(); currentRenderTargetViews.push_back(((WindowDX11*)window)->getBackBufferRtv());
    currentDepthStencilView = ((WindowDX11*)window)->getZBufferView();
    ((WindowDX11*)window)->getDxContext()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::setViewport(Rectanglei vp) {
    if (viewport.width()!=vp.width() ||
        viewport.height()!=vp.height() ||
        viewport.topLeftCorner().x!=vp.topLeftCorner().x ||
        viewport.topLeftCorner().y!=vp.topLeftCorner().y) {
        viewport = vp;

        dxViewport.Width = (FLOAT)viewport.width();
        dxViewport.Height = (FLOAT)viewport.height();
        dxViewport.MinDepth = 0.0f;
        dxViewport.MaxDepth = 1.0f;
        dxViewport.TopLeftX = viewport.topLeftCorner().x;
        dxViewport.TopLeftY = viewport.topLeftCorner().y;
        ((WindowDX11*)window)->getDxContext()->RSSetViewports( 1, &dxViewport );
    }
}

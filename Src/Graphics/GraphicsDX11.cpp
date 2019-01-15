#include "GraphicsDX11.h"
#include <Window/Window.h>
#include "../Window/WindowDX11.h"
#include <stdlib.h>

using namespace PGE;

Graphics* Graphics::create(int w,int h,bool fs) {
    return new GraphicsDX11(w,h,fs);
}

GraphicsDX11::GraphicsDX11(int w,int h,bool fs) {
    window = new WindowDX11("PGE",w,h,fs);

    ID3D11Device* dxDevice = ((WindowDX11*)window)->getDxDevice();
    ID3D11DeviceContext* dxContext = ((WindowDX11*)window)->getDxContext();

    viewport = Rectanglei(0,0,w,h);
    updateViewport();
    currentRenderTargetView = ((WindowDX11*)window)->getBackBufferRtv();
    currentDepthStencilView = ((WindowDX11*)window)->getZBufferView();
}

GraphicsDX11::~GraphicsDX11() {
    delete window;
}

void GraphicsDX11::update() {
    Graphics::update();
    updateViewport();
}

void GraphicsDX11::updateViewport() {
    if (currentViewport.width()!=viewport.width() ||
        currentViewport.height()!=viewport.height() ||
        currentViewport.topLeftCorner().x!=viewport.topLeftCorner().x ||
        currentViewport.topLeftCorner().y!=viewport.topLeftCorner().y) {
        currentViewport = viewport;
        //TODO: how do viewport
    }
}

void GraphicsDX11::clear(Color color) {
    updateViewport();

    float clearColor[4] = {color.red,color.green,color.blue,color.alpha};
    ((WindowDX11*)window)->getDxContext()->ClearRenderTargetView( currentRenderTargetView, clearColor );
    ((WindowDX11*)window)->getDxContext()->ClearDepthStencilView( currentDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.f, 0 );
}

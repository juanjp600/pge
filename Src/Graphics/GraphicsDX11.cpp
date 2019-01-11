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

    projectionMatrix = Matrix4x4f::identity;
    viewMatrix = Matrix4x4f::identity;
    Matrix4x4f tempWorldMat = Matrix4x4f::identity;

    FLOAT cbufferData[48];
    memcpy(cbufferData,projectionMatrix.elements,sizeof(FLOAT)*16);
    memcpy(cbufferData+16,viewMatrix.elements,sizeof(FLOAT)*16);
    memcpy(cbufferData+32,tempWorldMat.elements,sizeof(FLOAT)*16);

    ZeroMemory( &dxMatrixCBufferDesc, sizeof(D3D11_BUFFER_DESC) );
    dxMatrixCBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    dxMatrixCBufferDesc.ByteWidth = sizeof(FLOAT)*48;
    dxMatrixCBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    dxMatrixCBufferDesc.CPUAccessFlags = 0;

    ZeroMemory( &dxMatrixCBufferData, sizeof(D3D11_SUBRESOURCE_DATA) );
    dxMatrixCBufferData.pSysMem = cbufferData;

    dxDevice->CreateBuffer(&dxMatrixCBufferDesc,&dxMatrixCBufferData,&dxMatrixCBuffer);
}

void GraphicsDX11::updateDxCBuffer(Matrix4x4f worldMatrix) {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)window)->getDxContext();

    FLOAT cbufferData[48];
    memcpy(cbufferData,projectionMatrix.transpose().elements,sizeof(FLOAT)*16);
    memcpy(cbufferData+16,viewMatrix.transpose().elements,sizeof(FLOAT)*16);
    memcpy(cbufferData+32,worldMatrix.transpose().elements,sizeof(FLOAT)*16);

    dxContext->UpdateSubresource(dxMatrixCBuffer,0,NULL,cbufferData,0,0);
}

GraphicsDX11::~GraphicsDX11() {
    dxMatrixCBuffer->Release();
    
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

void GraphicsDX11::useMatrixCBuffer() {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)window)->getDxContext();
    dxContext->VSSetConstantBuffers(0,1,&dxMatrixCBuffer);
}

void GraphicsDX11::clear(Color color) {
    updateViewport();

    float clearColor[4] = {color.red,color.green,color.blue,color.alpha};
    ((WindowDX11*)window)->getDxContext()->ClearRenderTargetView( currentRenderTargetView, clearColor );
    ((WindowDX11*)window)->getDxContext()->ClearDepthStencilView( currentDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.f, 0 );
}

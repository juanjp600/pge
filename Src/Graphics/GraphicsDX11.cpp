#include "GraphicsDX11.h"

#include "../Texture/TextureDX11.h"

#include <SDL_syswm.h>
#include <stdlib.h>

using namespace PGE;

//REMINDER: https://code.msdn.microsoft.com/windowsdesktop/Direct3D-Tutorial-Win32-829979ef

GraphicsDX11::GraphicsDX11(String name,int w,int h,bool fs) : GraphicsInternal(name, w, h, fs) {
    HRESULT hResult = 0;
    int errorCode = 0;

    sdlWindow = std::shared_ptr<SDL_Window>(SDL_CreateWindow(name.cstr(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN),
        [](SDL_Window* w) { SDL_DestroyWindow(w); });
    if (sdlWindow == nullptr) {
        throw Exception("GraphicsDX11", "Failed to create SDL window: " + String(SDL_GetError()));
    }

    if (fullscreen) {
        SDL_SetWindowBordered(sdlWindow.get(), SDL_bool::SDL_FALSE);
        SDL_Rect displayBounds;
        int displayIndex = SDL_GetWindowDisplayIndex(sdlWindow.get());
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
        SDL_SetWindowSize(sdlWindow.get(), displayBounds.w, displayBounds.h);
        SDL_SetWindowPosition(sdlWindow.get(), 0, 0);
    }

    destructor.addPointer(dxgiFactory = new SmartDeviceChild<IDXGIFactory1>());

    hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (LPVOID*)(&dxgiFactory));
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create DXGI factory (HRESULT " + String::fromInt(hResult) + ")");
    }

    SDL_SysWMinfo sysWMinfo;
    SDL_VERSION(&sysWMinfo.version); //REMINDER: THIS LINE IS VERY IMPORTANT
    bool validInfo = SDL_GetWindowWMInfo(sdlWindow.get(), &sysWMinfo);
    if (!validInfo) {
        throw Exception("GraphicsDX11", "Failed to initialize SDL version info: " + String(SDL_GetError()));
    }

    destructor.addPointer(dxSwapChain = new SmartDeviceChild<IDXGISwapChain>());

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

    D3D_FEATURE_LEVEL dxFeatureLevel[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_9_3 };

    destructor.addPointer(dxContext = new SmartDeviceChild<ID3D11DeviceContext>());

    hResult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, dxFeatureLevel, 2, D3D11_SDK_VERSION,
        &(*dxDevice), NULL, &(*dxContext));

    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create D3D11 device (HRESULT " + String::fromInt(hResult) + ")");
    }

    IDXGIDevice1* dxgiDevice = nullptr;
    hResult = (*dxDevice)()->QueryInterface(__uuidof(IDXGIDevice1), (LPVOID*)(&dxgiDevice));
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to initialize DXGI device (HRESULT " + String::fromInt(hResult) + ")");
    }

    hResult = (*dxgiFactory)()->CreateSwapChain(dxgiDevice, &dxSwapChainDesc, &(*dxSwapChain));
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create DXGI swapchain (HRESULT " + String::fromInt(hResult) + ")");
    }

    dxgiDevice->Release();

    destructor.addPointer(dxBackBufferRtv = new SmartDeviceChild<ID3D11RenderTargetView>());

    ID3D11Texture2D* backBuffer;
    hResult = (*dxSwapChain)()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to retrieve back buffer (HRESULT " + String::fromInt(hResult) + ")");
    }
    hResult = (*dxDevice)()->CreateRenderTargetView(backBuffer, NULL, &(*dxBackBufferRtv));
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create back buffer target view (HRESULT " + String::fromInt(hResult) + ")");
    }
    backBuffer->Release();

    // Create depth stencil texture
    destructor.addPointer(dxZBufferTexture = new SmartDeviceChild<ID3D11Texture2D>());
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
    hResult = (*dxDevice)()->CreateTexture2D(&descDepth, NULL, &(*dxZBufferTexture));
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create main depth stencil texture (HRESULT " + String::fromInt(hResult) + ")");
    }

    // Create the depth stencil view
    destructor.addPointer(dxZBufferView = new SmartDeviceChild<ID3D11DepthStencilView>());
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hResult = (*dxDevice)()->CreateDepthStencilView((*dxZBufferTexture)(), &descDSV, &(*dxZBufferView));
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create main depth stencil view (HRESULT " + String::fromInt(hResult) + ")");
    }

    (*dxContext)()->OMSetRenderTargets(1, &(*dxBackBufferRtv), (*dxZBufferView)());

    destructor.addPointer(dxRasterizerState = new SmartDeviceChild<ID3D11RasterizerState>());

    ZeroMemory(&dxRasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));
    dxRasterizerStateDesc.AntialiasedLineEnable = false;
    dxRasterizerStateDesc.CullMode = D3D11_CULL_BACK;
    dxRasterizerStateDesc.DepthClipEnable = true;
    dxRasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
    dxRasterizerStateDesc.ScissorEnable = false;
    dxRasterizerStateDesc.MultisampleEnable = false;
    dxRasterizerStateDesc.FrontCounterClockwise = true;

    hResult = (*dxDevice)()->CreateRasterizerState(&dxRasterizerStateDesc, &(*dxRasterizerState));
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create main rasterizer state (HRESULT " + String::fromInt(hResult) + ")");
    }
    (*dxContext)()->RSSetState((*dxRasterizerState)());

    destructor.addPointer(dxBlendState = new SmartDeviceChild<ID3D11BlendState>());

    ZeroMemory(&dxBlendStateDesc, sizeof(D3D11_BLEND_DESC));
    dxBlendStateDesc.RenderTarget[0].BlendEnable = true;
    dxBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    dxBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    dxBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    dxBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    dxBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    dxBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    dxBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hResult = (*dxDevice)()->CreateBlendState(&dxBlendStateDesc, &(*dxBlendState)());
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create main blend state (HRESULT " + String::fromInt(hResult) + ")");
    }

    (*dxContext)()->OMSetBlendState((*dxBlendState)(), 0, 0xffffffff);

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

    destructor.addPointer(dxDepthStencilState = new SmartPrimitiveArray<ID3D11DepthStencilState*>(3, [](ID3D11DepthStencilState* const& s) { s->Release(); }));
    //TODO
    hResult = (*dxDevice)()->CreateDepthStencilState(&depthStencilDesc, &(*dxDepthStencilState)[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE]);
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create ENABLED_WRITE depth stencil state (HRESULT " + String::fromInt(hResult) + ")");
    }
    (*dxContext)()->OMSetDepthStencilState((*dxDepthStencilState)[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE], 0);

    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

    hResult = (*dxDevice)()->CreateDepthStencilState(&depthStencilDesc, &(*dxDepthStencilState)[(int)ZBUFFER_STATE_INDEX::ENABLED_NOWRITE]);
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create ENABLED_NOWRITE depth stencil state (HRESULT " + String::fromInt(hResult) + ")");
    }

    depthStencilDesc.DepthEnable = FALSE;

    hResult = (*dxDevice)()->CreateDepthStencilState(&depthStencilDesc, &(*dxDepthStencilState)[(int)ZBUFFER_STATE_INDEX::DISABLED]);
    if (FAILED(hResult)) {
        throw Exception("GraphicsDX11", "Failed to create DISABLED depth stencil state (HRESULT " + String::fromInt(hResult) + ")");
    }

    setViewport(Rectanglei(0,0,w,h));
    currentRenderTargetViews.push_back((*dxBackBufferRtv)());
    currentDepthStencilView = (*dxZBufferView)();

    depthTest = true;
    vsync = true;
}

void GraphicsDX11::swap() {
    (*dxSwapChain)()->Present(vsync ? 1 : 0, 0);
}

void GraphicsDX11::clear(Color color) {
    float clearColor[4] = {color.red,color.green,color.blue,color.alpha};
    for (int i=0;i<currentRenderTargetViews.size();i++) {
        (*dxContext)()->ClearRenderTargetView( currentRenderTargetViews[i], clearColor );
    }
    (*dxContext)()->ClearDepthStencilView( currentDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.f, 0 );
}

void GraphicsDX11::setRenderTarget(Texture* renderTarget) {
    for (int i=0;i<currentRenderTargetViews.size();i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    (*dxContext)()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear(); currentRenderTargetViews.push_back(((TextureDX11*)renderTarget)->getRtv());
    currentDepthStencilView = ((TextureDX11*)renderTarget)->getZBufferView();
    (*dxContext)()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::setRenderTargets(std::vector<Texture*> renderTargets) {
    for (int i=0;i<currentRenderTargetViews.size();i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    (*dxContext)()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear();
    TextureDX11* maxSizeTexture = (TextureDX11*)renderTargets[0];
    for (int i=0;i<renderTargets.size();i++) {
        if (!renderTargets[i]->isRenderTarget()) {
            throw Exception("setRenderTargets","renderTargets["+String::fromInt(i)+"] is not a valid render target");
        }
        currentRenderTargetViews.push_back(((TextureDX11*)renderTargets[i])->getRtv());
        if (renderTargets[i]->getWidth()+renderTargets[i]->getHeight()>maxSizeTexture->getWidth()+maxSizeTexture->getHeight()) {
            maxSizeTexture = (TextureDX11*)renderTargets[i];
        }
    }
    for (int i=0;i<renderTargets.size();i++) {
        if (renderTargets[i]->getWidth()>maxSizeTexture->getWidth() || renderTargets[i]->getHeight()>maxSizeTexture->getHeight()) {
            throw Exception("setRenderTargets",
                "Render target sizes are incompatible ("+String::fromInt(maxSizeTexture->getWidth())+","+String::fromInt(maxSizeTexture->getHeight())+" vs "+
                                                         String::fromInt(renderTargets[i]->getWidth())+","+String::fromInt(renderTargets[i]->getHeight())+")");
        }
    }
    currentDepthStencilView = maxSizeTexture->getZBufferView();
    (*dxContext)()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::resetRenderTarget() {
    SmartPrimitive<GraphicsDX11>* lol;
    lol->operator->()->resetRenderTarget();

    for (int i=0;i<currentRenderTargetViews.size();i++) {
        currentRenderTargetViews[i] = nullptr;
    }
    (*dxContext)()->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), nullptr );

    currentRenderTargetViews.clear(); currentRenderTargetViews.push_back((*dxBackBufferRtv)());
    currentDepthStencilView = (*dxZBufferView)();
    dxContext->OMSetRenderTargets( currentRenderTargetViews.size(), currentRenderTargetViews.data(), currentDepthStencilView );
}

void GraphicsDX11::setViewport(Rectanglei vp) {
    if (vp != viewport) {
        viewport = vp;

        dxViewport.Width = (FLOAT)viewport.width();
        dxViewport.Height = (FLOAT)viewport.height();
        dxViewport.MinDepth = 0.0f;
        dxViewport.MaxDepth = 1.0f;
        dxViewport.TopLeftX = viewport.topLeftCorner().x;
        dxViewport.TopLeftY = viewport.topLeftCorner().y;
        (*dxContext)->RSSetViewports( 1, &dxViewport );
    }
}

ID3D11Device* GraphicsDX11::get(*dxDevice)() const {
    return (*dxDevice);
}

ID3D11DeviceContext* GraphicsDX11::get(*dxContext)() const {
    return (*dxContext);
}

ID3D11RenderTargetView* GraphicsDX11::getBackBufferRtv() const {
    return dxBackBufferRtv;
}

ID3D11DepthStencilView* GraphicsDX11::getZBufferView() const {
    return dxZBufferView;
}

void GraphicsDX11::setZBufferState(GraphicsDX11::ZBUFFER_STATE_INDEX index) {
    (*dxContext)->OMSetDepthStencilState((*dxDepthStencilState)[(int)index], 0);
}

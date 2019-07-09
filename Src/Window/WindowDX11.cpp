#include <Window/Window.h>
#include "WindowDX11.h"

#include <Misc/String.h>

#include <Shader/Shader.h>
#include <Material/Material.h>
#include <Mesh/Mesh.h>

#include "../Exception/Exception.h"
#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

//REMINDER: https://code.msdn.microsoft.com/windowsdesktop/Direct3D-Tutorial-Win32-829979ef

WindowDX11::WindowDX11(String c,int w,int h,bool fs) {
    caption = c;
    width = w; height = h; fullscreen = fs;

    dxgiFactory = nullptr;

    dxSwapChain = nullptr;

    dxDevice = nullptr;
    dxContext = nullptr;

    dxBackBufferRtv = nullptr;
    dxZBufferTexture = nullptr;
    dxZBufferView = nullptr;
    dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE] = nullptr;
    dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_NOWRITE] = nullptr;
    dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::DISABLED] = nullptr;

    dxRasterizerState = nullptr;

    dxBlendState = nullptr;

    HRESULT hResult = 0;
    int errorCode = 0;

    sdlWindow = SDL_CreateWindow(c.cstr(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,w,h,SDL_WINDOW_SHOWN);
    if (sdlWindow == nullptr) {
        throwException("WindowDX11","Failed to create SDL window: "+String(SDL_GetError()));
    }

    if (fullscreen) {
        SDL_SetWindowBordered(sdlWindow,SDL_bool::SDL_FALSE);
        SDL_Rect displayBounds;
        int displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
        if (displayIndex < 0) {
            throwException("WindowDX11","Failed to determine display index: "+String(SDL_GetError()));
        }
        errorCode = SDL_GetDisplayBounds(displayIndex,&displayBounds);
        if (errorCode < 0) {
            throwException("WindowDX11","Failed to get display bounds: "+String(SDL_GetError()));
        }
        if (displayBounds.w <= 0 || displayBounds.h <= 0) {
            throwException("WindowDX11","Display bounds are invalid ("+String(displayBounds.w)+", "+String(displayBounds.h)+")");
        }
        SDL_SetWindowSize(sdlWindow,displayBounds.w,displayBounds.h);
        SDL_SetWindowPosition(sdlWindow,0,0);
    }

    eventSubscriber = new SysEventsInternal::SubscriberInternal(this,SysEventsInternal::SubscriberInternal::EventType::WINDOW);
    SysEventsInternal::subscribe(eventSubscriber);

    hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1),(LPVOID*)(&dxgiFactory));
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create DXGI factory (HRESULT "+String(hResult,true)+")");
    }

    SDL_SysWMinfo sysWMinfo;
    SDL_VERSION(&sysWMinfo.version); //REMINDER: THIS LINE IS VERY IMPORTANT
    bool validInfo = SDL_GetWindowWMInfo(sdlWindow,&sysWMinfo);
    if (!validInfo) {
        throwException("WindowDX11","Failed to initialize SDL version info: "+String(SDL_GetError()));
    }

    ZeroMemory( &dxSwapChainDesc, sizeof(dxSwapChainDesc) );
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

    hResult = D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_HARDWARE,NULL,0,dxFeatureLevel,2,D3D11_SDK_VERSION,
                      &dxDevice,NULL,&dxContext);

    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create D3D11 device (HRESULT "+String(hResult,true)+")");
    }

    IDXGIDevice* dxgiDevice = nullptr;
    hResult = dxDevice->QueryInterface(__uuidof(IDXGIDevice),(LPVOID*)(&dxgiDevice));
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to initialize DXGI device (HRESULT "+String(hResult,true)+")");
    }

    hResult = dxgiFactory->CreateSwapChain(dxgiDevice,&dxSwapChainDesc,&dxSwapChain);
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create DXGI swapchain (HRESULT "+String(hResult,true)+")");
    }

    dxgiDevice->Release();

    ID3D11Texture2D* backBuffer;
    hResult = dxSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (LPVOID*)&backBuffer);
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to retrieve back buffer (HRESULT "+String(hResult,true)+")");
    }
    hResult = dxDevice->CreateRenderTargetView(backBuffer, NULL, &dxBackBufferRtv);
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create back buffer target view (HRESULT "+String(hResult,true)+")");
    }
    backBuffer->Release();

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
    hResult = dxDevice->CreateTexture2D(&descDepth, NULL, &dxZBufferTexture);
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create main depth stencil texture (HRESULT "+String(hResult,true)+")");
    }
    
    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hResult = dxDevice->CreateDepthStencilView(dxZBufferTexture, &descDSV, &dxZBufferView);
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create main depth stencil view (HRESULT "+String(hResult,true)+")");
    }

    dxContext->OMSetRenderTargets(1, &dxBackBufferRtv, dxZBufferView);

    ZeroMemory(&dxRasterizerStateDesc,sizeof(D3D11_RASTERIZER_DESC));
    dxRasterizerStateDesc.AntialiasedLineEnable = false;
    dxRasterizerStateDesc.CullMode = D3D11_CULL_BACK;
    dxRasterizerStateDesc.DepthClipEnable = true;
    dxRasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
    dxRasterizerStateDesc.ScissorEnable = false;
    dxRasterizerStateDesc.MultisampleEnable = false;
    dxRasterizerStateDesc.FrontCounterClockwise = true;

    hResult = dxDevice->CreateRasterizerState(&dxRasterizerStateDesc,&dxRasterizerState);
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create main rasterizer state (HRESULT "+String(hResult,true)+")");
    }
    dxContext->RSSetState(dxRasterizerState);

    ZeroMemory( &dxBlendStateDesc,sizeof(D3D11_BLEND_DESC) );
    dxBlendStateDesc.RenderTarget[0].BlendEnable = true;
    dxBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    dxBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    dxBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    dxBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    dxBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    dxBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    dxBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hResult = dxDevice->CreateBlendState(&dxBlendStateDesc,&dxBlendState);
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create main blend state (HRESULT "+String(hResult,true)+")");
    }

    dxContext->OMSetBlendState(dxBlendState,0,0xffffffff);

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

    hResult = dxDevice->CreateDepthStencilState(&depthStencilDesc, &dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE]);
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create ENABLED_WRITE depth stencil state (HRESULT "+String(hResult,true)+")");
    }
    dxContext->OMSetDepthStencilState(dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE], 0);

    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

    hResult = dxDevice->CreateDepthStencilState(&depthStencilDesc, &dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_NOWRITE]);
    if (FAILED(hResult)) {
        throwException("WindowDX11","Failed to create ENABLED_NOWRITE depth stencil state (HRESULT "+String(hResult,true)+")");
    }

    depthStencilDesc.DepthEnable = FALSE;

    hResult = dxDevice->CreateDepthStencilState(&depthStencilDesc, &dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::DISABLED]);
    if (FAILED(hResult)) {
        throwException("WindowDX11", "Failed to create DISABLED depth stencil state (HRESULT " + String(hResult, true) + ")");
    }

    open = true;
    focused = true;
}

WindowDX11::~WindowDX11() {
    cleanup();
}

void WindowDX11::cleanup() {
    SysEventsInternal::unsubscribe(eventSubscriber);
    if (dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE]!=nullptr) {
        dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE]->Release();
    }
    if (dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_NOWRITE] != nullptr) {
        dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_NOWRITE]->Release();
    }
    if (dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::DISABLED] != nullptr) {
        dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::DISABLED]->Release();
    }
    if (dxRasterizerState!=nullptr) { dxRasterizerState->Release(); }
    if (dxBlendState!=nullptr) { dxBlendState->Release(); }
    if (dxContext!=nullptr) { dxContext->Release(); }
    if (dxZBufferView!=nullptr) { dxZBufferView->Release(); }
    if (dxZBufferTexture!=nullptr) { dxZBufferTexture->Release(); }
    if (dxBackBufferRtv!=nullptr) { dxBackBufferRtv->Release(); }
    if (dxSwapChain!=nullptr) { dxSwapChain->Release(); }
    if (dxgiFactory!=nullptr) { dxgiFactory->Release(); }
    if (dxDevice!=nullptr) { dxDevice->Release(); }
    if (sdlWindow!=nullptr) { SDL_DestroyWindow(sdlWindow); }

    dxgiFactory = nullptr;
    dxSwapChain = nullptr;
    dxDevice = nullptr;
    dxContext = nullptr;
    dxBackBufferRtv = nullptr;
    dxZBufferTexture = nullptr;
    dxZBufferView = nullptr;
    dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_WRITE] = nullptr;
    dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::ENABLED_NOWRITE] = nullptr;
    dxDepthStencilState[(int)ZBUFFER_STATE_INDEX::DISABLED] = nullptr;
    dxRasterizerState = nullptr;
    dxBlendState = nullptr;
    sdlWindow = nullptr;
}

void WindowDX11::throwException(String func,String details) {
    cleanup();
    throw Exception("WindowDX11::"+func,details);
}

void WindowDX11::update() {
    SDL_Event event;
    while (((SysEventsInternal::SubscriberInternal*)eventSubscriber)->popEvent(event)) {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            open = false;
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            focused = true;
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            focused = false;
        }
    }
}

void WindowDX11::swap(bool vsyncEnabled) {
    HRESULT hResult = dxSwapChain->Present(vsyncEnabled ? 1 : 0, 0);
    if (FAILED(hResult)) {
        throwException("swap","Failed to present (HRESULT "+String(hResult)+")");
    }
}

ID3D11Device* WindowDX11::getDxDevice() const {
    return dxDevice;
}

ID3D11DeviceContext* WindowDX11::getDxContext() const {
    return dxContext;
}

ID3D11RenderTargetView* WindowDX11::getBackBufferRtv() const {
    return dxBackBufferRtv;
}

ID3D11DepthStencilView* WindowDX11::getZBufferView() const {
    return dxZBufferView;
}

void WindowDX11::setZBufferState(WindowDX11::ZBUFFER_STATE_INDEX index) {
    dxContext->OMSetDepthStencilState(dxDepthStencilState[(int)index], 0);
}

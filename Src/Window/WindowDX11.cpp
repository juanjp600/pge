#include <Window/Window.h>
#include "WindowDX11.h"

#include <Misc/String.h>

#include <Shader/Shader.h>
#include <Material/Material.h>
#include <Mesh/Mesh.h>

using namespace PGE;

//REMINDER: https://code.msdn.microsoft.com/windowsdesktop/Direct3D-Tutorial-Win32-829979ef

WindowDX11::WindowDX11(String c,int w,int h,bool fs) {
    caption = c;
    width = w; height = h; fullscreen = fs;

    HRESULT hr;

    sdlWindow = SDL_CreateWindow(c.cstr(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,w,h,SDL_WINDOW_SHOWN);
    
    if (fullscreen) {
        SDL_SetWindowBordered(sdlWindow,SDL_bool::SDL_FALSE);
        SDL_Rect displayBounds;
        int displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
        SDL_GetDisplayBounds(displayIndex,&displayBounds);
        SDL_SetWindowSize(sdlWindow,displayBounds.w,displayBounds.h);
        SDL_SetWindowPosition(sdlWindow,0,0);
    }

    eventSubscriber = SysEvents::Subscriber(sdlWindow,SysEvents::Subscriber::EventType::WINDOW);
    SysEvents::subscribe(eventSubscriber);

    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1),(LPVOID*)(&dxgiFactory));
    if (FAILED(hr)) {
        SDL_Log("Failed to create DXGI factory!\n");
    }

    SDL_SysWMinfo sysWMinfo;
    SDL_VERSION(&sysWMinfo.version); //REMINDER: THIS LINE IS VERY IMPORTANT
    SDL_GetWindowWMInfo(sdlWindow,&sysWMinfo);

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

    D3D_FEATURE_LEVEL dxFeatureLevel = D3D_FEATURE_LEVEL_11_0;

    D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_HARDWARE,NULL,D3D11_CREATE_DEVICE_DEBUG,&dxFeatureLevel,1,D3D11_SDK_VERSION,
                      &dxDevice,NULL,&dxContext);

    IDXGIDevice* dxgiDevice = nullptr;
    dxDevice->QueryInterface(__uuidof(IDXGIDevice),(LPVOID*)(&dxgiDevice));

    hr = dxgiFactory->CreateSwapChain(dxgiDevice,&dxSwapChainDesc,&dxSwapChain);
    if (FAILED(hr)) {
        SDL_Log("Failed to create swapchain!\n");
    }

    dxgiDevice->Release();

    ID3D11Texture2D* backBuffer;
    dxSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&backBuffer );
    dxDevice->CreateRenderTargetView( backBuffer, NULL, &dxBackBufferRtv );
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
    hr = dxDevice->CreateTexture2D(&descDepth, NULL, &dxZBufferTexture);
    
    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = dxDevice->CreateDepthStencilView(dxZBufferTexture, &descDSV, &dxZBufferView);

    dxContext->OMSetRenderTargets( 1, &dxBackBufferRtv, dxZBufferView );

    ZeroMemory( &dxRasterizerStateDesc,sizeof(D3D11_RASTERIZER_DESC) );
    dxRasterizerStateDesc.AntialiasedLineEnable = false;
    dxRasterizerStateDesc.CullMode = D3D11_CULL_BACK;
    dxRasterizerStateDesc.DepthClipEnable = true;
    dxRasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
    dxRasterizerStateDesc.ScissorEnable = false;
    dxRasterizerStateDesc.MultisampleEnable = false;
    dxRasterizerStateDesc.FrontCounterClockwise = true;

    dxDevice->CreateRasterizerState( &dxRasterizerStateDesc,&dxRasterizerState );
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

    dxDevice->CreateBlendState(&dxBlendStateDesc,&dxBlendState);

    dxContext->OMSetBlendState(dxBlendState,0,0xffffffff);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    dxDevice->CreateDepthStencilState(&depthStencilDesc, &dxDepthStencilState[0]);
    dxContext->OMSetDepthStencilState(dxDepthStencilState[0], 0);

    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

    dxDevice->CreateDepthStencilState(&depthStencilDesc, &dxDepthStencilState[1]);
    //dxContext->OMSetDepthStencilState(dxDepthStencilState[1], 0);

    open = true;
    focused = true;
}

WindowDX11::~WindowDX11() {
    SysEvents::unsubscribe(eventSubscriber);

    dxDepthStencilState[0]->Release();
    dxDepthStencilState[1]->Release();
    dxRasterizerState->Release();
    dxBlendState->Release();
    dxContext->Release();
    dxZBufferView->Release();
    dxZBufferTexture->Release();
    dxBackBufferRtv->Release();
    dxSwapChain->Release();
    dxgiFactory->Release();
    dxDevice->Release();

    SDL_DestroyWindow(sdlWindow);
}

void WindowDX11::update() {
    SDL_Event event;
    while (eventSubscriber.popEvent(event)) {
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
    dxSwapChain->Present( vsyncEnabled ? 1 : 0, 0 );
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

void WindowDX11::setZBufferWriteState(bool enabled) {
    dxContext->OMSetDepthStencilState(dxDepthStencilState[enabled ? 0 : 1], 0);
}

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
    SDL_Log("%p\n",sysWMinfo.info.win.window);
    dxSwapChainDesc.SampleDesc.Count = 1;
    dxSwapChainDesc.SampleDesc.Quality = 0;
    dxSwapChainDesc.Windowed = TRUE;

    D3D_FEATURE_LEVEL dxFeatureLevel = D3D_FEATURE_LEVEL_11_0;

    /*D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &dxFeatureLevel, 1,
        D3D11_SDK_VERSION, &dxSwapChainDesc, &dxSwapChain, &dxDevice, NULL, &dxContext );*/

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
    dxContext->OMSetRenderTargets( 1, &dxBackBufferRtv, NULL );

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

    dxViewport.Width = (FLOAT)width;
    dxViewport.Height = (FLOAT)height;
    dxViewport.MinDepth = 0.0f;
    dxViewport.MaxDepth = 1.0f;
    dxViewport.TopLeftX = 0;
    dxViewport.TopLeftY = 0;
    dxContext->RSSetViewports( 1, &dxViewport );

    open = true;
}

WindowDX11::~WindowDX11() {
    SysEvents::unsubscribe(eventSubscriber);

    dxRasterizerState->Release();
    dxBlendState->Release();
    dxContext->Release();
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

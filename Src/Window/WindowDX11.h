#ifndef PGEINTERNAL_WINDOW_DX11_H_INCLUDED
#define PGEINTERNAL_WINDOW_DX11_H_INCLUDED

#include <Window/Window.h>
#include <Misc/String.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

namespace PGE {

class WindowDX11 : public Window {
    public:
        WindowDX11(String c,int w,int h,bool fs);
        ~WindowDX11();

        virtual void update();
        virtual void swap(bool vsyncEnabled=true);

        ID3D11Device* getDxDevice() const;
        ID3D11DeviceContext* getDxContext() const;
        ID3D11RenderTargetView* getBackBufferRtv() const;
    private:
        WindowDX11(){};

        IDXGIFactory1* dxgiFactory;
        DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
        IDXGISwapChain* dxSwapChain;
        ID3D11Device* dxDevice;
        ID3D11DeviceContext* dxContext;
        ID3D11RenderTargetView* dxBackBufferRtv;
        D3D11_RASTERIZER_DESC dxRasterizerStateDesc;
        ID3D11RasterizerState* dxRasterizerState;
        D3D11_BLEND_DESC dxBlendStateDesc;
        ID3D11BlendState* dxBlendState;
        D3D11_VIEWPORT dxViewport;
};

}

#endif

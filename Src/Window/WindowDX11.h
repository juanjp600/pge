#ifndef PGEINTERNAL_WINDOW_DX11_H_INCLUDED
#define PGEINTERNAL_WINDOW_DX11_H_INCLUDED

#include <Window/Window.h>
#include <Misc/String.h>

#include <SDL_syswm.h>

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
        ID3D11DepthStencilView* getZBufferView() const;

        void setZBufferWriteState(bool enabled);
    private:
        WindowDX11(){};

        void throwException(String func,String details);
        void cleanup();

        IDXGIFactory1* dxgiFactory;

        DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
        IDXGISwapChain* dxSwapChain;
        
        ID3D11Device* dxDevice;
        ID3D11DeviceContext* dxContext;
        
        ID3D11RenderTargetView* dxBackBufferRtv;
        ID3D11Texture2D* dxZBufferTexture;
        ID3D11DepthStencilView* dxZBufferView;
        ID3D11DepthStencilState* dxDepthStencilState[2];
        
        D3D11_RASTERIZER_DESC dxRasterizerStateDesc;
        ID3D11RasterizerState* dxRasterizerState;
        
        D3D11_BLEND_DESC dxBlendStateDesc;
        ID3D11BlendState* dxBlendState;
};

}

#endif

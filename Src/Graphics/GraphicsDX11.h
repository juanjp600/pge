#ifndef PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

#include "GraphicsInternal.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

namespace PGE {

class GraphicsDX11 : public GraphicsInternal {
    public:
        GraphicsDX11(String name,int w,int h,bool fs);
        ~GraphicsDX11() override;

        virtual void swap() override;

        virtual void clear(Color color) override;

        virtual void setRenderTarget(Texture* renderTarget) override;
        virtual void setRenderTargets(std::vector<Texture*> renderTargets) override;
        virtual void resetRenderTarget() override;

        virtual void setViewport(Rectanglei vp) override;

        ID3D11Device* getDxDevice() const;
        ID3D11DeviceContext* getDxContext() const;
        ID3D11RenderTargetView* getBackBufferRtv() const;
        ID3D11DepthStencilView* getZBufferView() const;

        enum class ZBUFFER_STATE_INDEX {
            ENABLED_WRITE = 0,
            ENABLED_NOWRITE = 1,
            DISABLED = 2
        };

        void setZBufferState(ZBUFFER_STATE_INDEX index);

    private:
        virtual void cleanup() override;

        IDXGIFactory1* dxgiFactory;

        DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
        IDXGISwapChain* dxSwapChain;

        ID3D11Device* dxDevice;
        ID3D11DeviceContext* dxContext;

        ID3D11RenderTargetView* dxBackBufferRtv;
        ID3D11Texture2D* dxZBufferTexture;
        ID3D11DepthStencilView* dxZBufferView;
        ID3D11DepthStencilState* dxDepthStencilState[3];

        D3D11_RASTERIZER_DESC dxRasterizerStateDesc;
        ID3D11RasterizerState* dxRasterizerState;

        D3D11_BLEND_DESC dxBlendStateDesc;
        ID3D11BlendState* dxBlendState;

        D3D11_VIEWPORT dxViewport;

        std::vector<ID3D11RenderTargetView*> currentRenderTargetViews;
        ID3D11DepthStencilView* currentDepthStencilView;
};

}

#endif // PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

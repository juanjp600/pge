#ifndef PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

#include "GraphicsInternal.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <ResourceManagement/ResourceReferenceVector.h>
#include "../ResourceManagement/DX11.h"

namespace PGE {

class GraphicsDX11 : public GraphicsInternal {
    public:
        GraphicsDX11(String name,int w,int h,bool fs);

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

        __GFX_OBJ_DEC

    private:
        DXGIFactory1::Ref dxgiFactory;

        DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
        DXGISwapChain::Ref dxSwapChain;

        D3D11Device::Ref dxDevice;
        D3D11ImmediateContext::Ref dxContext;

        D3D11RenderTargetView::Ref dxBackBufferRtv;
        D3D11Texture2D::Ref dxZBufferTexture;
        D3D11DepthStencilView::Ref dxZBufferView;
        ResourceReferenceVector<ID3D11DepthStencilState*> dxDepthStencilState;

        D3D11_RASTERIZER_DESC dxRasterizerStateDesc;
        D3D11RasterizerState::Ref dxRasterizerState;

        D3D11_BLEND_DESC dxBlendStateDesc;
        D3D11BlendState::Ref dxBlendState;

        D3D11_VIEWPORT dxViewport;

        ResourceReferenceVector<ID3D11RenderTargetView*> currentRenderTargetViews;
        D3D11DepthStencilView::Ref currentDepthStencilView;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

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
        GraphicsDX11(const String& name,int w,int h,bool fs);

        void swap() override;

        void clear(Color color) override;

        void setRenderTarget(Texture* renderTarget) override;
        void setRenderTargets(const std::vector<Texture*>& renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(const Rectanglei& vp) override;

        ID3D11Device* getDxDevice() const;
        ID3D11DeviceContext* getDxContext() const;
        ID3D11RenderTargetView* getBackBufferRtv() const;
        ID3D11DepthStencilView* getZBufferView() const;

        enum class ZBufferStateIndex {
            ENABLED_WRITE = 0,
            ENABLED_NOWRITE = 1,
            DISABLED = 2
        };

        void setZBufferState(ZBufferStateIndex index);

        PGE_GFX_OBJ_DEC

    private:
        DXGIFactory1::View dxgiFactory;

        DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
        DXGISwapChain::View dxSwapChain;

        D3D11Device::View dxDevice;
        D3D11ImmediateContext::View dxContext;

        D3D11RenderTargetView::View dxBackBufferRtv;
        D3D11Texture2D::View dxZBufferTexture;
        D3D11DepthStencilView::View dxZBufferView;
        ResourceReferenceVector<ID3D11DepthStencilState*> dxDepthStencilState;

        D3D11_RASTERIZER_DESC dxRasterizerStateDesc;
        D3D11RasterizerState::View dxRasterizerState;

        D3D11_BLEND_DESC dxBlendStateDesc;
        D3D11BlendState::View dxBlendState;

        D3D11_VIEWPORT dxViewport;

        ResourceReferenceVector<ID3D11RenderTargetView*> currentRenderTargetViews;
        D3D11DepthStencilView::View currentDepthStencilView;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

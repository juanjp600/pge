#ifndef PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

#include "GraphicsInternal.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <ResourceManagement/ResourceReference.h>
#include <ResourceManagement/ResourceManager.h>
#include <ResourceManagement/ResourceOwner.h>

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

        D3D11DeviceRef getDxDevice() const;
        D3D11DeviceContextRef getDxContext() const;
        D3D11RenderTargetViewRef getBackBufferRtv() const;
        D3D11DepthStencilViewRef getZBufferView() const;

        enum class ZBUFFER_STATE_INDEX {
            ENABLED_WRITE = 0,
            ENABLED_NOWRITE = 1,
            DISABLED = 2
        };

        void setZBufferState(ZBUFFER_STATE_INDEX index);

    private:
        DXGIFactory1Ref dxgiFactory;

        DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
        DXGISwapChainRef dxSwapChain;

        D3D11DeviceRef dxDevice;
        D3D11DeviceContextRef dxContext;

        D3D11RenderTargetViewRef dxBackBufferRtv;
        D3D11Texture2DRef dxZBufferTexture;
        D3D11DepthStencilViewRef dxZBufferView;
        ResourceRefVector<ID3D11DepthStencilState*> dxDepthStencilState;

        D3D11_RASTERIZER_DESC dxRasterizerStateDesc;
        D3D11RasterizerStateRef dxRasterizerState;

        D3D11_BLEND_DESC dxBlendStateDesc;
        D3D11BlendStateRef dxBlendState;

        D3D11_VIEWPORT dxViewport;

        ResourceRefVector<ID3D11RenderTargetView*> currentRenderTargetViews;
        D3D11DepthStencilViewRef currentDepthStencilView;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

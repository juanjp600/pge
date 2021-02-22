#ifndef PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

#include "GraphicsInternal.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include "../Misc/SmartPrimitive.h"

namespace PGE {

class GraphicsDX11 : public GraphicsInternal {
    public:
        GraphicsDX11(String name,int w,int h,bool fs);
        virtual ~GraphicsDX11() {};

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

        static void destroyChild(ID3D11DeviceChild* const& child) {
            child->Release();
        }

        static void destroyChildren(std::vector<ID3D11DeviceChild*> const& children) {
            for (auto& c : children) {
                c->Release();
            }
        }

    private:
        SmartRef<IDXGIFactory1*> dxgiFactory;

        DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
        SmartRef<IDXGISwapChain*> dxSwapChain;

        SmartRef<ID3D11Device*> dxDevice;
        SmartRef<ID3D11DeviceContext*> dxContext;

        SmartRef<ID3D11RenderTargetView*> dxBackBufferRtv;
        SmartRef<ID3D11Texture2D*> dxZBufferTexture;
        SmartRef<ID3D11DepthStencilView*> dxZBufferView;
        SmartRef<std::vector<ID3D11DepthStencilState*>> dxDepthStencilState;

        D3D11_RASTERIZER_DESC dxRasterizerStateDesc;
        SmartRef<ID3D11RasterizerState*> dxRasterizerState;

        D3D11_BLEND_DESC dxBlendStateDesc;
        SmartRef<ID3D11BlendState*> dxBlendState;

        SmartOrderedDestructor destructor = 10;

        D3D11_VIEWPORT dxViewport;

        std::vector<ID3D11RenderTargetView*> currentRenderTargetViews;
        ID3D11DepthStencilView* currentDepthStencilView;
};

}

#endif // PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

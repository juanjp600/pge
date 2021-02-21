#ifndef PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

#include "GraphicsInternal.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include "../Misc/SmartPrimitive.h"

template <typename T>
class SmartDeviceChild : public SmartPrimitive<T*, ID3D11DeviceChild*> {
    public:
    SmartDeviceChild() {
        SmartPrimitive<T*, ID3D11DeviceChild*>::value = nullptr;
    }

    ~SmartDeviceChild() {
        if (SmartPrimitive<T*, ID3D11DeviceChild*>::value != nullptr) {
            SmartPrimitive<T*, ID3D11DeviceChild*>::value->Release();
        }
    }
};

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

    private:
        SmartDeviceChild<IDXGIFactory1>* dxgiFactory;

        DXGI_SWAP_CHAIN_DESC dxSwapChainDesc;
        SmartDeviceChild<IDXGISwapChain>* dxSwapChain;

        SmartDeviceChild<ID3D11Device>* dxDevice;
        SmartDeviceChild<ID3D11DeviceContext>* dxContext;

        SmartDeviceChild<ID3D11RenderTargetView>* dxBackBufferRtv;
        SmartDeviceChild<ID3D11Texture2D>* dxZBufferTexture;
        SmartDeviceChild<ID3D11DepthStencilView>* dxZBufferView;
        SmartPrimitiveArray<ID3D11DepthStencilState*>* dxDepthStencilState;

        D3D11_RASTERIZER_DESC dxRasterizerStateDesc;
        SmartDeviceChild<ID3D11RasterizerState>* dxRasterizerState;

        D3D11_BLEND_DESC dxBlendStateDesc;
        SmartDeviceChild<ID3D11BlendState>* dxBlendState;

        D3D11_VIEWPORT dxViewport;

        std::vector<ID3D11RenderTargetView*> currentRenderTargetViews;
        ID3D11DepthStencilView* currentDepthStencilView;

        SmartOrderedDestructor destructor;
};

}

#endif // PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

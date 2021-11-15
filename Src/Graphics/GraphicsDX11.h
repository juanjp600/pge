#ifndef PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

#include "GraphicsInternal.h"

#include "Shader/ShaderDX11.h"
#include "Mesh/MeshDX11.h"
#include "Texture/TextureDX11.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include "../ResourceManagement/DX11.h"

namespace PGE {

enum class ZBufferStateIndex {
    ENABLED_WRITE = 0,
    ENABLED_NOWRITE = 1,
    DISABLED = 2
};

class GraphicsDX11 : public GraphicsSpecialized<ShaderDX11, MeshDX11, TextureDX11> {
    public:
        GraphicsDX11(const String& name, int w, int h, WindowMode wm, int x, int y);

        void swap() override;

        void clear(const Color& color) override;

        void setRenderTarget(Texture& renderTarget) override;
        void setRenderTargets(const ReferenceVector<Texture>& renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(const Rectanglei& vp) override;

        void setCulling(CullingMode mode) override;

        ID3D11Device* getDxDevice() const;
        ID3D11DeviceContext* getDxContext() const;
        ID3D11RenderTargetView* getBackBufferRtv() const;
        ID3D11DepthStencilView* getZBufferView() const;

        void setZBufferState(ZBufferStateIndex index);

    private:
        DXGIFactory1::View dxgiFactory;

        DXGISwapChain::View dxSwapChain;

        D3D11Device::View dxDevice;
        D3D11ImmediateContext::View dxContext;

        D3D11RenderTargetView::View dxBackBufferRtv;
        D3D11Texture2D::View dxZBufferTexture;
        D3D11DepthStencilView::View dxZBufferView;
        std::vector<ID3D11DepthStencilState*> dxDepthStencilState;

        D3D11RasterizerState::View dxRasterizerState;

        D3D11BlendState::View dxBlendState;

        D3D11_VIEWPORT dxViewport;

        std::vector<ID3D11RenderTargetView*> currentRenderTargetViews;
        ID3D11DepthStencilView* currentDepthStencilView;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

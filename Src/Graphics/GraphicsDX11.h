#ifndef PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <Graphics/Graphics.h>
#include <Shader/Shader.h>

namespace PGE {

class GraphicsDX11 : public Graphics {
    public:
        GraphicsDX11(String name,int w=1280,int h=720,bool fs=false);
        ~GraphicsDX11();

        virtual Renderer getRenderer() override;

        virtual void update() override;

        virtual void clear(Color color) override;

        virtual void setDepthTest(bool enabled) override;
        virtual bool getDepthTest() const override;

        virtual void setRenderTarget(Texture* renderTarget) override;
        virtual void setRenderTargets(std::vector<Texture*> renderTargets) override;
        virtual void resetRenderTarget() override;

        virtual void setViewport(Rectanglei vp) override;
    private:
        void throwException(String func,String details) override;
        void cleanup() override;

        std::vector<ID3D11RenderTargetView*> currentRenderTargetViews;
        ID3D11DepthStencilView* currentDepthStencilView;

        D3D11_VIEWPORT dxViewport;

        bool depthTestEnabled;
};

}

#endif // PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

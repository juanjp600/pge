#ifndef PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

#include <Graphics/Graphics.h>
#include <Shader/Shader.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

namespace PGE {

class ShaderDX11;
class GraphicsDX11 : public Graphics {
    public:
        GraphicsDX11(String name,int w=1280,int h=720,bool fs=false);
        ~GraphicsDX11();

        virtual void update();

        virtual void clear(Color color);

        virtual void setRenderTarget(Texture* renderTarget);
        virtual void setRenderTargets(std::vector<Texture*> renderTargets);
        virtual void resetRenderTarget();

        virtual void setViewport(Rectanglei vp);
    private:
        void throwException(String func,String details) override;
        void cleanup() override;

        std::vector<ID3D11RenderTargetView*> currentRenderTargetViews;
        ID3D11DepthStencilView* currentDepthStencilView;

        D3D11_VIEWPORT dxViewport;
};

}

#endif

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
        GraphicsDX11(int w=1280,int h=720,bool fs=false);
        ~GraphicsDX11();

        virtual void update();
        void updateViewport();
        void updateDxCBuffer(Matrix4x4f worldMatrix);
        //void useVertexInputLayout();
        void useMatrixCBuffer();
        //void useSampler();

        virtual void clear(Color color);

        ID3D11RenderTargetView* currentRenderTargetView;
    private:
        Rectanglei currentViewport;

        D3D11_BUFFER_DESC dxMatrixCBufferDesc;
        D3D11_SUBRESOURCE_DATA dxMatrixCBufferData;
        ID3D11Buffer* dxMatrixCBuffer;
};

}

#endif

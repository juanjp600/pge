#ifndef PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_DX11_H_INCLUDED

#include <Graphics/Graphics.h>

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
        void useVertexInputLayout();
        void useMatrixCBuffer();
        void useSampler();

        virtual void clear(Color color);

        ID3D11RenderTargetView* currentRenderTargetView;

        Shader* getDefaultShader();
    private:
        Rectanglei currentViewport;

        D3D11_INPUT_ELEMENT_DESC dxVertexInputElemDesc[4];
        ID3D11InputLayout* dxVertexInputLayout;

        D3D11_BUFFER_DESC dxMatrixCBufferDesc;
        D3D11_SUBRESOURCE_DATA dxMatrixCBufferData;
        ID3D11Buffer* dxMatrixCBuffer;

        D3D11_SAMPLER_DESC dxSamplerDesc;
        ID3D11SamplerState* dxSamplerState;

        ShaderDX11* defaultShader;
};

}

#endif

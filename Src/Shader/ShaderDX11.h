#ifndef PGEINTERNAL_SHADER_DX11_H_INCLUDED
#define PGEINTERNAL_SHADER_DX11_H_INCLUDED

#include <Graphics/Graphics.h>
#include <Shader/Shader.h>
#include <Misc/String.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <inttypes.h>
#include <vector>

namespace PGE {

class ShaderDX11 : public Shader {
    public:
        ShaderDX11(Graphics* gfx,const String& path);
        ~ShaderDX11();

        void useShader();
        void useVertexInputLayout();
        void useSamplers();

        uint8_t* getDxVsCode(); int getDxVsCodeLen() const;
        uint8_t* getDxFsCode(); int getDxFsCodeLen() const;

        enum class VERTEX_INPUT_ELEM {
            POSITION,
            NORMAL,
            TEXCOORD,
            COLOR
        };
        const std::vector<VERTEX_INPUT_ELEM>& getVertexInputElems() const;
    private:
        ShaderDX11(){};

        std::vector<uint8_t> vertexShaderBytecode;
        std::vector<uint8_t> fragmentShaderBytecode;

        std::vector<D3D11_INPUT_ELEMENT_DESC> dxVertexInputElemDesc;
        ID3D11InputLayout* dxVertexInputLayout;

        std::vector<VERTEX_INPUT_ELEM> vertexInputElems;

        std::vector<ID3D11SamplerState*> dxSamplerState;

        ID3D11VertexShader* dxVertexShader;
        ID3D11PixelShader* dxFragmentShader;

        Graphics* graphics;

};

}

#endif

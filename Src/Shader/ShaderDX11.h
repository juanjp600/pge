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

        uint8_t* getDxVsCode(); int getDxVsCodeLen() const;
        uint8_t* getDxFsCode(); int getDxFsCodeLen() const;
    private:
        ShaderDX11(){};

        std::vector<uint8_t> vertexShaderBytecode;
        std::vector<uint8_t> fragmentShaderBytecode;

        ID3D11VertexShader* dxVertexShader;
        ID3D11PixelShader* dxFragmentShader;

        Graphics* graphics;

};

}

#endif

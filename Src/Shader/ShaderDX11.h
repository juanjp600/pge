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
        ShaderDX11(Graphics* gfx,const FilePath& path);
        virtual ~ShaderDX11();

        Constant* getVertexShaderConstant(String name);
        Constant* getFragmentShaderConstant(String name);

        void useShader();
        void useVertexInputLayout();
        void useSamplers();

        uint8_t* getDxVsCode(); int getDxVsCodeLen() const;
        uint8_t* getDxFsCode(); int getDxFsCodeLen() const;

        const std::vector<String>& getVertexInputElems() const;
    private:
        ShaderDX11(){};

        void cleanup() override;
        void throwException(String func, String details) override;

        std::vector<uint8_t> vertexShaderBytecode;
        std::vector<uint8_t> fragmentShaderBytecode;

        std::vector<D3D11_INPUT_ELEMENT_DESC> dxVertexInputElemDesc;
        ID3D11InputLayout* dxVertexInputLayout;

        std::vector<String> vertexInputElems;

        class CBufferInfo;
        class ConstantDX11 : public Constant {
            public:
                ConstantDX11(CBufferInfo* cBuffer, String nm, int offst, int sz);
                ~ConstantDX11(){};

                void setValue(Matrix4x4f value) override;
                void setValue(Vector2f value) override;
                void setValue(Vector3f value) override;
                void setValue(Vector4f value) override;
                void setValue(Color value) override;
                void setValue(float value) override;
                void setValue(int value) override;

                String getName() const;
            private:
                virtual void throwException(String func, String details);

                CBufferInfo* constantBuffer;
                String name;
                int offset;
                int size;
        };
        class CBufferInfo {
            public:
                CBufferInfo(Graphics* graphics, String nm,int sz);
                ~CBufferInfo();

                uint8_t* getData();
                std::vector<ConstantDX11>& getConstants();
                void addConstant(ConstantDX11 constant);
                bool isDirty() const;
                void markAsDirty();
                void update();
                ID3D11Buffer* getDxCBuffer();
            private:
                String name;
                uint8_t* data;
                int size;
                std::vector<ConstantDX11> constants;
                ID3D11DeviceContext* dxContext;
                ID3D11Buffer* dxCBuffer;
                bool dirty;
        };
        std::vector<CBufferInfo*> vertexConstantBuffers;
        std::vector<CBufferInfo*> fragmentConstantBuffers;
        void readConstantBuffers(std::ifstream& reflectionInfo, std::vector<CBufferInfo*>& constantBuffers);

        std::vector<ID3D11SamplerState*> dxSamplerState;

        ID3D11VertexShader* dxVertexShader;
        ID3D11PixelShader* dxFragmentShader;

        Graphics* graphics;

};

}

#endif

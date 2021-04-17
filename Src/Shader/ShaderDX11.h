#ifndef PGEINTERNAL_SHADER_DX11_H_INCLUDED
#define PGEINTERNAL_SHADER_DX11_H_INCLUDED

#include <vector>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <ResourceManagement/ResourceReference.h>
#include <Shader/Shader.h>
#include <Misc/String.h>

#include "../Graphics/GraphicsDX11.h"

namespace PGE {

class ShaderDX11 : public Shader {
    public:
        ShaderDX11(Graphics* gfx, const FilePath& path);

        Constant* getVertexShaderConstant(const String& name);
        Constant* getFragmentShaderConstant(const String& name);

        void useShader();
        void useVertexInputLayout();
        void useSamplers();

        uint8_t* getDxVsCode(); int getDxVsCodeLen() const;
        uint8_t* getDxFsCode(); int getDxFsCodeLen() const;

        const std::vector<String>& getVertexInputElems() const;

    private:
        std::vector<uint8_t> vertexShaderBytecode;
        std::vector<uint8_t> fragmentShaderBytecode;

        std::vector<String> vertexInputElemSemanticNames;
        std::vector<D3D11_INPUT_ELEMENT_DESC> dxVertexInputElemDesc;
        D3D11InputLayout::Ref dxVertexInputLayout;

        std::vector<String> vertexInputElems;

        class CBufferInfo;
        typedef ResourceReference<CBufferInfo*> CBufferInfoRef;
        class ConstantDX11 : public Constant {
            public:
                ConstantDX11(CBufferInfoRef cBuffer, const String& nm, int offst, int sz);
                ~ConstantDX11(){};

                void setValue(const Matrix4x4f& value) override;
                void setValue(const Vector2f& value) override;
                void setValue(const Vector3f& value) override;
                void setValue(const Vector4f& value) override;
                void setValue(const Color& value) override;
                void setValue(float value) override;
                void setValue(int value) override;

                const String& getName() const;

            private:
                CBufferInfoRef constantBuffer;
                String name;
                int offset;
                int size;
        };

        class CBufferInfo {
            public:
                CBufferInfo(Graphics* graphics, const String& nm, int sz, ResourceManager* resourceManager);
                ~CBufferInfo();

                uint8_t* getData();
                std::vector<ConstantDX11>& getConstants();
                void addConstant(ConstantDX11 constant);
                bool isDirty() const;
                void markAsDirty();
                void update();
                D3D11Buffer::Ref getDxCBuffer();

            private:
                String name;
                uint8_t* data;
                int size;
                std::vector<ConstantDX11> constants;
                D3D11ImmediateContext::Ref dxContext;
                D3D11Buffer::Ref dxCBuffer;
                bool dirty;
        };
        
        class CBufferInfoOwner : public Resource<CBufferInfo*> {
            public:
                CBufferInfoOwner(Graphics* gfx, const String& nm, int sz, ResourceManager* rm);

                __RES_MNGMT__REF_FACT_METH(CBufferInfoOwner)
        };

        ResourceReferenceVector<CBufferInfo*> vertexConstantBuffers;
        ResourceReferenceVector<CBufferInfo*> fragmentConstantBuffers;
        void readConstantBuffers(std::ifstream& reflectionInfo, ResourceReferenceVector<CBufferInfo*>& constantBuffers);

        ResourceReferenceVector<ID3D11SamplerState*> dxSamplerState;

        D3D11VertexShader::Ref dxVertexShader;
        D3D11PixelShader::Ref dxFragmentShader;

        Graphics* graphics;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_SHADER_DX11_H_INCLUDED

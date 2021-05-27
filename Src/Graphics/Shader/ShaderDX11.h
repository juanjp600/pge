#ifndef PGEINTERNAL_SHADER_DX11_H_INCLUDED
#define PGEINTERNAL_SHADER_DX11_H_INCLUDED

#include <unordered_map>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <ResourceManagement/ResourceView.h>
#include <Graphics/Shader.h>
#include <String/String.h>

#include "../GraphicsDX11.h"

namespace PGE {

class ShaderDX11 : public Shader {
    public:
        ShaderDX11(Graphics* gfx, const FilePath& path);

        Constant* getVertexShaderConstant(const String& name);
        Constant* getFragmentShaderConstant(const String& name);

        void useShader();
        void useVertexInputLayout();
        void useSamplers();

        byte* getDxVsCode(); int getDxVsCodeLen() const;
        byte* getDxFsCode(); int getDxFsCodeLen() const;

        const std::vector<String>& getVertexInputElems() const;

    private:
        std::vector<byte> vertexShaderBytecode;
        std::vector<byte> fragmentShaderBytecode;

        std::vector<String> vertexInputElemSemanticNames;
        std::vector<D3D11_INPUT_ELEMENT_DESC> dxVertexInputElemDesc;
        D3D11InputLayout::View dxVertexInputLayout;

        std::vector<String> vertexInputElems;

        class CBufferInfo;
        typedef ResourceView<CBufferInfo*> CBufferInfoRef;
        class ConstantDX11 : public Constant {
            public:
                ConstantDX11(CBufferInfoRef cBuffer, int offst, int sz);

                void setValue(const Matrix4x4f& value) override;
                void setValue(const Vector2f& value) override;
                void setValue(const Vector3f& value) override;
                void setValue(const Vector4f& value) override;
                void setValue(const Color& value) override;
                void setValue(float value) override;
                void setValue(int value) override;

            private:
                CBufferInfoRef constantBuffer;
                int offset;
                int size;
        };

        class CBufferInfo {
            public:
                CBufferInfo(Graphics* graphics, const String& nm, int sz, ResourceManager* resourceManager);
                ~CBufferInfo();

                byte* getData();
                std::unordered_map<String::Key, ConstantDX11>* getConstants();
                void addConstant(const String& name, const ConstantDX11& constant);
                bool isDirty() const;
                void markAsDirty();
                void update();
                D3D11Buffer::View getDxCBuffer();

            private:
                String name;
                byte* data;
                int size;
                std::unordered_map<String::Key, ConstantDX11> constants;
                D3D11ImmediateContext::View dxContext;
                D3D11Buffer::View dxCBuffer;
                bool dirty;
        };
        
        // TODO: Move.
        class CBufferInfoOwner : public Resource<CBufferInfo*> {
            public:
                CBufferInfoOwner(Graphics* gfx, const String& nm, int sz, ResourceManager* rm);
        };

        ResourceViewVector<CBufferInfo*> vertexConstantBuffers;
        ResourceViewVector<CBufferInfo*> fragmentConstantBuffers;
        void readConstantBuffers(std::ifstream& reflectionInfo, ResourceViewVector<CBufferInfo*>& constantBuffers);

        ResourceViewVector<ID3D11SamplerState*> dxSamplerState;

        D3D11VertexShader::View dxVertexShader;
        D3D11PixelShader::View dxFragmentShader;

        Graphics* graphics;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_SHADER_DX11_H_INCLUDED

#ifndef PGEINTERNAL_SHADER_DX11_H_INCLUDED
#define PGEINTERNAL_SHADER_DX11_H_INCLUDED

#include <unordered_map>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <PGE/ResourceManagement/ResourceView.h>
#include <PGE/File/BinaryReader.h>
#include <PGE/Graphics/Shader.h>

#include "../GraphicsDX11.h"

namespace PGE {

class ShaderDX11 : public Shader {
    public:
        ShaderDX11(Graphics* gfx, const FilePath& path);

        Constant* getVertexShaderConstant(const String& name) override;
        Constant* getFragmentShaderConstant(const String& name) override;

        void useShader();
        void useVertexInputLayout();
        void useSamplers();

        const std::vector<String>& getVertexInputElems() const;

    private:
        D3D11InputLayout::View dxVertexInputLayout;

        std::vector<String> vertexInputElems;

        class CBufferInfo;
        typedef ResourceView<CBufferInfo*> CBufferInfoView;
        class ConstantDX11 : public Constant {
            public:
                ConstantDX11(CBufferInfoView cBuffer, int offst, int sz);

                void setValue(const Matrix4x4f& value) override;
                void setValue(const Vector2f& value) override;
                void setValue(const Vector3f& value) override;
                void setValue(const Vector4f& value) override;
                void setValue(const Color& value) override;
                void setValue(float value) override;
                void setValue(int value) override;

            private:
                CBufferInfoView constantBuffer;
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
        
        // TODO: Rid the world of this atrocity.
        class CBufferInfoOwner : public Resource<CBufferInfo*> {
            public:
                CBufferInfoOwner(Graphics* gfx, const String& nm, int sz, ResourceManager* rm);
        };

        ResourceViewVector<CBufferInfo*> vertexConstantBuffers;
        ResourceViewVector<CBufferInfo*> fragmentConstantBuffers;
        void readConstantBuffers(BinaryReader& reflectionInfo, ResourceViewVector<CBufferInfo*>& constantBuffers);

        ResourceViewVector<ID3D11SamplerState*> dxSamplerState;

        D3D11VertexShader::View dxVertexShader;
        D3D11PixelShader::View dxFragmentShader;

        Graphics* graphics;

        ResourceManager resourceManager;
};

}

#endif // PGEINTERNAL_SHADER_DX11_H_INCLUDED

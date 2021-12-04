#ifndef PGEINTERNAL_SHADER_DX11_H_INCLUDED
#define PGEINTERNAL_SHADER_DX11_H_INCLUDED

#include <unordered_map>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <PGE/ResourceManagement/ResourceView.h>
#include <PGE/File/BinaryReader.h>
#include <PGE/Graphics/Shader.h>

#include "../../ResourceManagement/DX11.h"

namespace PGE {

class GraphicsDX11;
class ShaderDX11 : public Shader {
    public:
        ShaderDX11(const Graphics& gfx, const FilePath& path);

        Constant* getVertexShaderConstant(const String& name) override;
        Constant* getFragmentShaderConstant(const String& name) override;

        void useShader();
        void useVertexInputLayout();
        void useSamplers();

    private:
        GraphicsDX11& graphics;

        D3D11InputLayout::View dxVertexInputLayout;

        class CBufferInfo;
        class ConstantDX11 : public Constant {
            public:
                ConstantDX11(CBufferInfo& cBuffer, int offst, int sz);

            private:
                CBufferInfo& constantBuffer;

                int offset;
                int size;

                void setValueInternal(const std::span<byte>& data);
        };

        class CBufferInfo {
            public:
                CBufferInfo() = delete;
                CBufferInfo(const CBufferInfo& other) = delete;
                CBufferInfo(const GraphicsDX11& gfx, const String& nm, int sz, ResourceManager& resourceManager);
                ~CBufferInfo();

                CBufferInfo(CBufferInfo&& other) noexcept;
                void operator=(CBufferInfo&& other) noexcept;

                byte* getData();
                std::unordered_map<String::Key, ConstantDX11>& getConstants();
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
                ID3D11DeviceContext* dxContext;
                D3D11Buffer::View dxCBuffer;
                bool dirty;
        };
        Constant* findConstantInBuffers(std::vector<CBufferInfo>& asd, const String& name);

        std::vector<CBufferInfo> vertexConstantBuffers;
        std::vector<CBufferInfo> fragmentConstantBuffers;
        void readConstantBuffers(BinaryReader& reflectionInfo, std::vector<CBufferInfo>& constantBuffers);

        std::vector<ID3D11SamplerState*> dxSamplerState;

        D3D11VertexShader::View dxVertexShader;
        D3D11PixelShader::View dxFragmentShader;

        ResourceManager resourceManager;

        int dxgiFormatToByteSize(DXGI_FORMAT dxgiFormat);
};

}

#endif // PGEINTERNAL_SHADER_DX11_H_INCLUDED

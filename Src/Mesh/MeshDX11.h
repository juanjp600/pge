#ifndef PGEINTERNAL_MESH_DX11_H_INCLUDED
#define PGEINTERNAL_MESH_DX11_H_INCLUDED

#include <Windows.h>

#include <Mesh/Mesh.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <vector>

namespace PGE {

/*struct DxVertex {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 uv;
    DirectX::XMFLOAT4 color;
};*/

class MeshDX11 : public Mesh {
    public:
        MeshDX11(Graphics* gfx,Primitive::TYPE pt);
        ~MeshDX11();

        virtual void render();
    private:
        std::vector<FLOAT> dxVertexData;
        std::vector<WORD> dxIndexData;
        
        D3D11_BUFFER_DESC dxVertexBufferDesc;
        D3D11_SUBRESOURCE_DATA dxVertexBufferData;
        ID3D11Buffer* dxVertexBuffer; 

        D3D11_BUFFER_DESC dxIndexBufferDesc;
        D3D11_SUBRESOURCE_DATA dxIndexBufferData;
        ID3D11Buffer* dxIndexBuffer;

        virtual void updateInternalData();

        Graphics* graphics;
};

}

#endif

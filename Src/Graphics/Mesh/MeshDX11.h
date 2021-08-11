#ifndef PGEINTERNAL_MESH_DX11_H_INCLUDED
#define PGEINTERNAL_MESH_DX11_H_INCLUDED

#include <PGE/Graphics/Mesh.h>

#include <vector>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <Windows.h>

#include "../../ResourceManagement/DX11.h"

namespace PGE {

class MeshDX11 : public Mesh, private GraphicsReferencer<class GraphicsDX11> {
    public:
        MeshDX11(Graphics& gfx);

    private:
        D3D11_BUFFER_DESC dxVertexBufferDesc;
        D3D11_SUBRESOURCE_DATA dxVertexBufferData;
        D3D11Buffer::View dxVertexBuffer;

        D3D11_BUFFER_DESC dxIndexBufferDesc;
        D3D11_SUBRESOURCE_DATA dxIndexBufferData;
        ResourceView<ID3D11Buffer*> dxIndexBuffer;

        ResourceManager resourceManager;

        void uploadInternalData() override;
        void renderInternal() override;
};

}

#endif // PGEINTERNAL_MESH_DX11_H_INCLUDED

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

class GraphicsDX11;
class MeshDX11 : public Mesh {
    public:
        MeshDX11(Graphics& gfx);

    private:
        GraphicsDX11& graphics;

        D3D11Buffer::View dxVertexBuffer;
        ResourceView<ID3D11Buffer*> dxIndexBuffer;

        ResourceManager resourceManager;

        void uploadInternalData() override;
        void renderInternal() override;
};

}

#endif // PGEINTERNAL_MESH_DX11_H_INCLUDED

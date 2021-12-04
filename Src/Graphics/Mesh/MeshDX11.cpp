#include "../GraphicsDX11.h"

#include <PGE/Types/Range.h>

using namespace PGE;

MeshDX11::MeshDX11(Graphics& gfx) : graphics((GraphicsDX11&)gfx) { }

void MeshDX11::uploadInternalData() {
    ID3D11Device* dxDevice = graphics.getDxDevice();

    if (vertices.getDataSize() > 0) {
        resourceManager.deleteResource(dxVertexBuffer);
        dxVertexBuffer = resourceManager.addNewResource<D3D11Buffer>(dxDevice, D3D11Buffer::Type::VERTEX, (void*)vertices.getData(), vertices.getDataSize());
    }

    if (indices.size() > 0) {
        resourceManager.deleteResource(dxIndexBuffer);
        dxIndexBuffer = resourceManager.addNewResource<D3D11Buffer>(dxDevice, D3D11Buffer::Type::INDEX, indices.data(), sizeof(u32) * (UINT)indices.size());
    }
}

void MeshDX11::renderInternal() {
    ID3D11DeviceContext* dxContext = graphics.getDxContext();

    if (!dxVertexBuffer.isHoldingResource() || !dxIndexBuffer.isHoldingResource()) { return; }

    ((ShaderDX11&)material->getShader()).useVertexInputLayout();

    UINT offset = 0; UINT stride = vertices.getLayout().getElementSize();
    dxContext->IASetVertexBuffers(0, 1, &dxVertexBuffer, &stride, &offset);
    dxContext->IASetIndexBuffer(dxIndexBuffer, DXGI_FORMAT_R32_UINT,0);

    D3D11_PRIMITIVE_TOPOLOGY dxPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    if (primitiveType == PrimitiveType::LINE) {
        dxPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    }

    dxContext->IASetPrimitiveTopology(dxPrimitiveTopology);

    ShaderDX11& shader = ((ShaderDX11&)material->getShader());

    shader.useShader();
    shader.useSamplers();
    for (int i : Range(material->getTextureCount())) {
        ((TextureDX11&)material->getTexture(i)).useTexture(i);
    }

    using enum ZBufferStateIndex;
    graphics.setZBufferState(
        graphics.getDepthTest()
                ? (isOpaque() ? ENABLED_WRITE : ENABLED_NOWRITE)
                : DISABLED);
    
    dxContext->DrawIndexed((UINT)indices.size(), 0, 0);
}

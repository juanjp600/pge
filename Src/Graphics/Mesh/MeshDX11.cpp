#include "../GraphicsDX11.h"

using namespace PGE;

MeshDX11::MeshDX11(Graphics* gfx) {
    graphics = gfx;
}

GraphicsDX11* PGE::MeshDX11::getGraphicsDx11() const
{
    return (GraphicsDX11*)graphics;
}

void MeshDX11::uploadInternalData() {
    if (!mustReuploadInternalData) { return; }

    ID3D11Device* dxDevice = getGraphicsDx11()->getDxDevice();

    if (vertices.getData().size() > 0) {
        ZeroMemory(&dxVertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
        dxVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        dxVertexBufferDesc.ByteWidth = (UINT)vertices.getData().size();
        dxVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        dxVertexBufferDesc.CPUAccessFlags = 0;

        ZeroMemory(&dxVertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
        dxVertexBufferData.pSysMem = vertices.getData().data();

        resourceManager.deleteResource(dxVertexBuffer);
        dxVertexBuffer = resourceManager.addNewResource<D3D11Buffer>(dxDevice, dxVertexBufferDesc, dxVertexBufferData);
    }

    if (indices.size() > 0) {
        ZeroMemory(&dxIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
        dxIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        dxIndexBufferDesc.ByteWidth = sizeof(u32)*(UINT)indices.size();
        dxIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        dxIndexBufferDesc.CPUAccessFlags = 0;

        ZeroMemory(&dxIndexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
        dxIndexBufferData.pSysMem = indices.data();

        resourceManager.deleteResource(dxIndexBuffer);
        dxIndexBuffer = resourceManager.addNewResource<D3D11Buffer>(dxDevice, dxIndexBufferDesc, dxIndexBufferData);
    }

    mustReuploadInternalData = false;
}

void MeshDX11::render() {
    if (primitiveType == PrimitiveType::NONE) { return; }

    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    uploadInternalData();

    if (!dxVertexBuffer.isHoldingResource() || !dxIndexBuffer.isHoldingResource()) { return; }

    ((ShaderDX11*)material->getShader())->useVertexInputLayout();

    UINT offset = 0; UINT stride = vertices.getLayout().getElementSize();
    dxContext->IASetVertexBuffers(0,1,&dxVertexBuffer,&stride,&offset);
    dxContext->IASetIndexBuffer(dxIndexBuffer,DXGI_FORMAT_R32_UINT,0);

    D3D11_PRIMITIVE_TOPOLOGY dxPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    if (primitiveType==PrimitiveType::LINE) {
        dxPrimitiveTopology=D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    }

    dxContext->IASetPrimitiveTopology(dxPrimitiveTopology);

    ShaderDX11* shader = ((ShaderDX11*)material->getShader());

    shader->useShader();
    shader->useSamplers();
    for (int i=0;i<material->getTextureCount();i++) {
        ((TextureDX11*)material->getTexture(i))->useTexture(i);
    }

    getGraphicsDx11()->setZBufferState(
        graphics->getDepthTest()
                ? (isOpaque() ? GraphicsDX11::ZBufferStateIndex::ENABLED_WRITE : GraphicsDX11::ZBufferStateIndex::ENABLED_NOWRITE)
                : GraphicsDX11::ZBufferStateIndex::DISABLED);
    
    dxContext->DrawIndexed(indices.size(),0,0);

    ID3D11ShaderResourceView* nullResource = nullptr;
    for (int i=0;i<material->getTextureCount();i++) {
        dxContext->PSSetShaderResources(i,1,&nullResource);
    }
}

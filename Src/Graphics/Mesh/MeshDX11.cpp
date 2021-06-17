#include "MeshDX11.h"
#include <PGE/Graphics/Graphics.h>
#include "../GraphicsDX11.h"
#include <PGE/Graphics/Shader.h>
#include "../Shader/ShaderDX11.h"
#include <PGE/Graphics/Texture.h>
#include "../Texture/TextureDX11.h"
#include <PGE/Exception/Exception.h>

using namespace PGE;

MeshDX11::MeshDX11(Graphics* gfx,Primitive::Type pt) : resourceManager(2) {
    graphics = gfx;
    primitiveType = pt;
}

void MeshDX11::updateInternalData() {
    if (!mustUpdateInternalData) { return; }
    
    dxVertexData.clear(); dxIndexData.clear();

    bool recalculateStride = true;
    stride = 0;
    const std::vector<String>& vertexInputElems = ((ShaderDX11*)material->getShader())->getVertexInputElems();
    for (size_t i=0;i<vertices.size();i++) {
        for (size_t j=0;j<vertexInputElems.size();j++) {
            const Vertex::Property& prop = vertices[i].getProperty(vertexInputElems[j]);
            switch (prop.type) {
                case Vertex::Property::Type::FLOAT: {
                    if (recalculateStride) { stride += sizeof(float); }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+sizeof(float));
                    memcpy(&(dxVertexData[offset]),&(prop.value.floatVal),sizeof(float));
                } break;
                case Vertex::Property::Type::UINT: {
                    if (recalculateStride) { stride += sizeof(u32); }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+sizeof(u32));
                    u32 uint = prop.value.uintVal;
                    memcpy(&(dxVertexData[offset]),&uint,sizeof(u32));
                } break;
                case Vertex::Property::Type::VECTOR2F: {
                    if (recalculateStride) { stride += sizeof(float)*2; }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*2));
                    memcpy(&(dxVertexData[offset]),&(prop.value.vector2fVal.x),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.vector2fVal.y),sizeof(float));
                } break;
                case Vertex::Property::Type::VECTOR3F: {
                    if (recalculateStride) { stride += sizeof(float)*3; }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*3));
                    memcpy(&(dxVertexData[offset]),&(prop.value.vector3fVal.x),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.vector3fVal.y),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*2),&(prop.value.vector3fVal.z),sizeof(float));
                } break;
                case Vertex::Property::Type::VECTOR4F: {
                    if (recalculateStride) { stride += sizeof(float)*4; }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*4));
                    memcpy(&(dxVertexData[offset]),&(prop.value.vector4fVal.x),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.vector4fVal.y),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*2),&(prop.value.vector4fVal.z),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*3),&(prop.value.vector4fVal.w),sizeof(float));
                } break;
                case Vertex::Property::Type::COLOR: {
                    if (recalculateStride) { stride += sizeof(float)*4; }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*4));
                    memcpy(&(dxVertexData[offset]),&(prop.value.colorVal.red),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.colorVal.green),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*2),&(prop.value.colorVal.blue),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*3),&(prop.value.colorVal.alpha),sizeof(float));
                } break;
            }
        }
        recalculateStride = false;
    }

    for (size_t i=0;i<primitives.size();i++) {
        dxIndexData.push_back((WORD)primitives[i].a);
        dxIndexData.push_back((WORD)primitives[i].b);
        if (primitiveType==Primitive::Type::TRIANGLE) {
            dxIndexData.push_back((WORD)primitives[i].c);
        }
    }

    mustUpdateInternalData = false;
}

void MeshDX11::uploadInternalData() {
    if (mustUpdateInternalData) { updateInternalData(); }
    if (!mustReuploadInternalData) { return; }

    ID3D11Device* dxDevice = ((GraphicsDX11*)graphics)->getDxDevice();
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    if (dxVertexData.size() > 0) {
        ZeroMemory(&dxVertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
        dxVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        dxVertexBufferDesc.ByteWidth = (UINT)dxVertexData.size();
        dxVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        dxVertexBufferDesc.CPUAccessFlags = 0;

        ZeroMemory(&dxVertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
        dxVertexBufferData.pSysMem = dxVertexData.data();

        resourceManager.deleteResourcefromReference(dxVertexBuffer);
        dxVertexBuffer = resourceManager.addNewResource<D3D11Buffer>(dxDevice, dxVertexBufferDesc, dxVertexBufferData);
    }

    if (dxIndexData.size() > 0) {
        ZeroMemory(&dxIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
        dxIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        dxIndexBufferDesc.ByteWidth = sizeof(WORD)*(UINT)dxIndexData.size();
        dxIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        dxIndexBufferDesc.CPUAccessFlags = 0;

        ZeroMemory(&dxIndexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
        dxIndexBufferData.pSysMem = dxIndexData.data();

        resourceManager.deleteResourcefromReference(dxIndexBuffer);
        dxIndexBuffer = resourceManager.addNewResource<D3D11Buffer>(dxDevice, dxIndexBufferDesc, dxIndexBufferData);
    }

    mustReuploadInternalData = false;
}

void MeshDX11::render() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    updateInternalData();
    uploadInternalData();

    if (!dxVertexBuffer.isHoldingResource() || !dxIndexBuffer.isHoldingResource()) { return; }

    ((ShaderDX11*)material->getShader())->useVertexInputLayout();

    UINT offset = 0;
    dxContext->IASetVertexBuffers(0,1,&dxVertexBuffer,&stride,&offset);
    dxContext->IASetIndexBuffer(dxIndexBuffer,DXGI_FORMAT_R16_UINT,0);

    D3D11_PRIMITIVE_TOPOLOGY dxPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    int dxIndexMultiplier = 3;
    if (primitiveType==Primitive::Type::LINE) {
        dxPrimitiveTopology=D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        dxIndexMultiplier = 2;
    }

    dxContext->IASetPrimitiveTopology(dxPrimitiveTopology);

    ShaderDX11* shader = ((ShaderDX11*)material->getShader());

    shader->useShader();
    shader->useSamplers();
    for (int i=0;i<material->getTextureCount();i++) {
        ((TextureDX11*)material->getTexture(i))->useTexture(i);
    }

    ((GraphicsDX11*)graphics)->setZBufferState(
        graphics->getDepthTest()
                ? (isOpaque() ? GraphicsDX11::ZBufferStateIndex::ENABLED_WRITE : GraphicsDX11::ZBufferStateIndex::ENABLED_NOWRITE)
                : GraphicsDX11::ZBufferStateIndex::DISABLED);
    
    dxContext->DrawIndexed(primitives.size()*dxIndexMultiplier,0,0);

    ID3D11ShaderResourceView* nullResource = nullptr;
    for (int i=0;i<material->getTextureCount();i++) {
        dxContext->PSSetShaderResources(i,1,&nullResource);
    }
}

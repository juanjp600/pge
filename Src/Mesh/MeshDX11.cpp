#include "MeshDX11.h"
#include <Window/Window.h>
#include "../Window/WindowDX11.h"
#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsDX11.h"
#include <Shader/Shader.h>
#include "../Shader/ShaderDX11.h"
#include <Texture/Texture.h>
#include "../Texture/TextureDX11.h"
#include <Exception/Exception.h>

#include <inttypes.h>

using namespace PGE;

Mesh* Mesh::create(Graphics* gfx, Primitive::TYPE pt) {
    return new MeshDX11(gfx, pt);
}

MeshDX11::MeshDX11(Graphics* gfx,Primitive::TYPE pt) {
    graphics = gfx;

    primitiveType = pt;

    dxVertexData.clear();
    dxIndexData.clear();
    vertices.clear(); vertexCount = 0;
    primitives.clear(); primitiveCount = 0;
    dxVertexBuffer = nullptr;
    dxIndexBuffer = nullptr;
}

MeshDX11::~MeshDX11() {
    cleanup();
}

void MeshDX11::throwException(String func, String details) {
    cleanup();
    throw Exception("MeshDX11::" + func, details);
}

void MeshDX11::cleanup() {
    dxVertexData.clear();
    dxIndexData.clear();
    if (dxVertexBuffer != nullptr) { dxVertexBuffer->Release(); }
    if (dxIndexBuffer != nullptr) { dxIndexBuffer->Release(); }
    vertices.clear();
    primitives.clear();
    dxVertexBuffer = nullptr;
    dxIndexBuffer = nullptr;
}

void MeshDX11::updateInternalData() {
    if (!mustUpdateInternalData) { return; }
    
    dxVertexData.clear();
    dxIndexData.clear();

    bool recalculateStride = true;
    stride = 0;
    const std::vector<String>& vertexInputElems = ((ShaderDX11*)material->getShader())->getVertexInputElems();
    int vertexInputElemCount = (int)vertexInputElems.size();
    int* indexHints = new int[vertexInputElemCount];
    for (int j=0;j<vertexInputElemCount;j++) {
        indexHints[j] = 0;
    }
    for (int i=0;i<vertexCount;i++) {
        for (int j=0;j<vertexInputElemCount;j++) {
            const Vertex::Property& prop = vertices[i].getProperty(vertexInputElems[j],indexHints[j]);
            switch (prop.type) {
                case Vertex::PROPERTY_TYPE::FLOAT: {
                    if (recalculateStride) { stride += sizeof(float); }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+sizeof(float));
                    memcpy(&(dxVertexData[offset]),&(prop.value.floatVal),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::UINT: {
                    if (recalculateStride) { stride += sizeof(uint32_t); }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+sizeof(uint32_t));
                    uint32_t uint = prop.value.uintVal;
                    memcpy(&(dxVertexData[offset]),&uint,sizeof(uint32_t));
                } break;
                case Vertex::PROPERTY_TYPE::VECTOR2F: {
                    if (recalculateStride) { stride += sizeof(float)*2; }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*2));
                    memcpy(&(dxVertexData[offset]),&(prop.value.vector2fVal.x),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.vector2fVal.y),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::VECTOR3F: {
                    if (recalculateStride) { stride += sizeof(float)*3; }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*3));
                    memcpy(&(dxVertexData[offset]),&(prop.value.vector3fVal.x),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.vector3fVal.y),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*2),&(prop.value.vector3fVal.z),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::VECTOR4F: {
                    if (recalculateStride) { stride += sizeof(float)*4; }
                    int offset = (int)dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*4));
                    memcpy(&(dxVertexData[offset]),&(prop.value.vector4fVal.x),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.vector4fVal.y),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*2),&(prop.value.vector4fVal.z),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*3),&(prop.value.vector4fVal.w),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::COLOR: {
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
    delete[] indexHints;

    for (int i=0;i<primitiveCount;i++) {
        dxIndexData.push_back((WORD)primitives[i].a);
        dxIndexData.push_back((WORD)primitives[i].b);
        if (primitiveType==Primitive::TYPE::TRIANGLE) {
            dxIndexData.push_back((WORD)primitives[i].c);
        }
    }

    mustUpdateInternalData = false;
}

void MeshDX11::uploadInternalData() {
    if (mustUpdateInternalData) { updateInternalData(); }
    if (!mustReuploadInternalData) { return; }

    ID3D11Device* dxDevice = ((WindowDX11*)graphics->getWindow())->getDxDevice();
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

    if (dxVertexBuffer!=nullptr) {
        dxVertexBuffer->Release(); dxVertexBuffer=nullptr;
    }
    if (dxIndexBuffer!=nullptr) {
        dxIndexBuffer->Release(); dxIndexBuffer=nullptr;
    }

    HRESULT hResult = 0;

    if (dxVertexData.size() > 0) {
        ZeroMemory(&dxVertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
        dxVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        dxVertexBufferDesc.ByteWidth = (UINT)dxVertexData.size();
        dxVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        dxVertexBufferDesc.CPUAccessFlags = 0;

        ZeroMemory(&dxVertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
        dxVertexBufferData.pSysMem = dxVertexData.data();

        hResult = dxDevice->CreateBuffer(&dxVertexBufferDesc, &dxVertexBufferData, &dxVertexBuffer);
        if (FAILED(hResult)) {
            throwException("uploadInternalData", "Failed to create vertex buffer (vertex count: " + String(vertices.size(), false) + "; vertex data size: " + String(dxVertexData.size(), false) + "; HRESULT " + String((uint64_t)hResult, true) + ")");
        }
    }

    if (dxIndexData.size() > 0) {
        ZeroMemory(&dxIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
        dxIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        dxIndexBufferDesc.ByteWidth = sizeof(WORD)*(UINT)dxIndexData.size();
        dxIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        dxIndexBufferDesc.CPUAccessFlags = 0;

        ZeroMemory(&dxIndexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
        dxIndexBufferData.pSysMem = dxIndexData.data();

        hResult = dxDevice->CreateBuffer(&dxIndexBufferDesc, &dxIndexBufferData, &dxIndexBuffer);
        if (FAILED(hResult)) {
            throwException("uploadInternalData", "Failed to create index buffer (index count: " + String(dxIndexData.size(), false) + "; HRESULT " + String((uint64_t)hResult, true) + ")");
        }
    }

    mustReuploadInternalData = false;
}

void MeshDX11::render() {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

    updateInternalData();
    uploadInternalData();

    if (dxVertexBuffer == nullptr || dxIndexBuffer == nullptr) { return; }

    ((ShaderDX11*)material->getShader())->useVertexInputLayout();

    UINT offset = 0;
    dxContext->IASetVertexBuffers(0,1,&dxVertexBuffer,&stride,&offset);
    dxContext->IASetIndexBuffer(dxIndexBuffer,DXGI_FORMAT_R16_UINT,0);

    D3D11_PRIMITIVE_TOPOLOGY dxPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    int dxIndexMultiplier = 3;
    if (primitiveType==Primitive::TYPE::LINE) {
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

    ((WindowDX11*)graphics->getWindow())->setZBufferState(
        graphics->getDepthTest()
                ? (opaque ? WindowDX11::ZBUFFER_STATE_INDEX::ENABLED_WRITE : WindowDX11::ZBUFFER_STATE_INDEX::ENABLED_NOWRITE)
                : WindowDX11::ZBUFFER_STATE_INDEX::DISABLED);
    
    dxContext->DrawIndexed(primitiveCount*dxIndexMultiplier,0,0);

    ID3D11ShaderResourceView* nullResource = nullptr;
    for (int i=0;i<material->getTextureCount();i++) {
        dxContext->PSSetShaderResources(i,1,&nullResource);
    }
}

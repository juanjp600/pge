#include "MeshDX11.h"
#include <Window/Window.h>
#include "../Window/WindowDX11.h"
#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsDX11.h"
#include <Shader/Shader.h>
#include "../Shader/ShaderDX11.h"
#include <Texture/Texture.h>
#include "../Texture/TextureDX11.h"

#include <inttypes.h>

using namespace PGE;

Mesh* Mesh::create(Graphics* gfx, Primitive::TYPE pt) {
    return new MeshDX11(gfx, pt);
}

MeshDX11::MeshDX11(Graphics* gfx,Primitive::TYPE pt) {
    graphics = gfx;

    primitiveType = pt;

    dxVertexData.clear();
    dxVertexBuffer = nullptr;
    dxIndexBuffer = nullptr;
}

MeshDX11::~MeshDX11() {
    if (dxVertexBuffer!=nullptr) { dxVertexBuffer->Release(); }
    if (dxIndexBuffer!=nullptr) { dxIndexBuffer->Release(); }
}

void MeshDX11::updateInternalData() {
    if (!isDirty) { return; }

    dxVertexData.clear();
    dxIndexData.clear();
    
    bool recalculateStride = true;
    stride = 0;
    const std::vector<String>& vertexInputElems = ((ShaderDX11*)material->getShader())->getVertexInputElems();
    for (int i=0;i<vertices.size();i++) {
        for (int j=0;j<vertexInputElems.size();j++) {
            const Vertex::Property& prop = vertices[i].getProperty(vertexInputElems[j]);
            switch (prop.type) {
                case Vertex::PROPERTY_TYPE::FLOAT: {
                    if (recalculateStride) { stride += sizeof(float); }
                    int offset = dxVertexData.size();
                    dxVertexData.resize(offset+sizeof(float));
                    memcpy(&(dxVertexData[offset]),&(prop.value.floatVal),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::UINT: {
                    if (recalculateStride) { stride += sizeof(uint32_t); }
                    int offset = dxVertexData.size();
                    dxVertexData.resize(offset+sizeof(uint32_t));
                    uint32_t uint = prop.value.uintVal;
                    memcpy(&(dxVertexData[offset]),&uint,sizeof(uint32_t));
                } break;
                case Vertex::PROPERTY_TYPE::VECTOR2F: {
                    if (recalculateStride) { stride += sizeof(float)*2; }
                    int offset = dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*2));
                    memcpy(&(dxVertexData[offset]),&(prop.value.vector2fVal.x),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.vector2fVal.y),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::VECTOR3F: {
                    if (recalculateStride) { stride += sizeof(float)*3; }
                    int offset = dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*3));
                    memcpy(&(dxVertexData[offset]),&(prop.value.vector3fVal.x),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.vector3fVal.y),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*2),&(prop.value.vector3fVal.z),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::VECTOR4F: {
                    if (recalculateStride) { stride += sizeof(float)*4; }
                    int offset = dxVertexData.size();
                    dxVertexData.resize(offset+(sizeof(float)*4));
                    memcpy(&(dxVertexData[offset]),&(prop.value.vector4fVal.x),sizeof(float));
                    memcpy(&(dxVertexData[offset])+sizeof(float),&(prop.value.vector4fVal.y),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*2),&(prop.value.vector4fVal.z),sizeof(float));
                    memcpy(&(dxVertexData[offset])+(sizeof(float)*3),&(prop.value.vector4fVal.w),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::COLOR: {
                    if (recalculateStride) { stride += sizeof(float)*4; }
                    int offset = dxVertexData.size();
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

    for (int i=0;i<primitives.size();i++) {
        dxIndexData.push_back(primitives[i].a);
        dxIndexData.push_back(primitives[i].b);
        if (primitiveType==Primitive::TYPE::TRIANGLE) {
            dxIndexData.push_back(primitives[i].c);
        }
    }

    ID3D11Device* dxDevice = ((WindowDX11*)graphics->getWindow())->getDxDevice();
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

    if (dxVertexBuffer!=nullptr) {
        dxVertexBuffer->Release(); dxVertexBuffer=nullptr;
    }
    if (dxIndexBuffer!=nullptr) {
        dxIndexBuffer->Release(); dxIndexBuffer=nullptr;
    }

    ZeroMemory( &dxVertexBufferDesc, sizeof(D3D11_BUFFER_DESC) );
    dxVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    dxVertexBufferDesc.ByteWidth = dxVertexData.size();
    dxVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    dxVertexBufferDesc.CPUAccessFlags = 0;

    ZeroMemory( &dxVertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA) );
    dxVertexBufferData.pSysMem = dxVertexData.data();

    dxDevice->CreateBuffer(&dxVertexBufferDesc,&dxVertexBufferData,&dxVertexBuffer);

    ZeroMemory( &dxIndexBufferDesc, sizeof(D3D11_BUFFER_DESC) );
    dxIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    dxIndexBufferDesc.ByteWidth = sizeof(WORD)*dxIndexData.size();
    dxIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    dxIndexBufferDesc.CPUAccessFlags = 0;

    ZeroMemory( &dxIndexBufferData, sizeof(D3D11_SUBRESOURCE_DATA) );
    dxIndexBufferData.pSysMem = dxIndexData.data();

    dxDevice->CreateBuffer(&dxIndexBufferDesc,&dxIndexBufferData,&dxIndexBuffer);

    isDirty = false;
}

void MeshDX11::render() {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();

    updateInternalData();

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

    ((WindowDX11*)graphics->getWindow())->setZBufferWriteState(opaque);

    dxContext->DrawIndexed(primitives.size()*dxIndexMultiplier,0,0);
}

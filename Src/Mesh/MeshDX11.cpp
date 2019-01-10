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
    
    for (int i=0;i<vertices.size();i++) {
        dxVertexData.push_back(vertices[i].pos.x);
        dxVertexData.push_back(vertices[i].pos.y);
        dxVertexData.push_back(vertices[i].pos.z);

        dxVertexData.push_back(vertices[i].normal.x);
        dxVertexData.push_back(vertices[i].normal.y);
        dxVertexData.push_back(vertices[i].normal.z);

        dxVertexData.push_back(vertices[i].uv.x);
        dxVertexData.push_back(vertices[i].uv.y);

        dxVertexData.push_back(vertices[i].color.red);
        dxVertexData.push_back(vertices[i].color.green);
        dxVertexData.push_back(vertices[i].color.blue);
        dxVertexData.push_back(vertices[i].color.alpha);
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
    dxVertexBufferDesc.ByteWidth = sizeof(FLOAT)*dxVertexData.size();
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

    ((GraphicsDX11*)graphics)->updateDxCBuffer(worldMatrix);

    ((GraphicsDX11*)graphics)->useVertexInputLayout();

    UINT stride = sizeof(FLOAT)*12;
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

    ((ShaderDX11*)material->getShader())->useShader();
    ((GraphicsDX11*)graphics)->useMatrixCBuffer();
    ((GraphicsDX11*)graphics)->useSampler();
    ((TextureDX11*)material->getTexture())->useTexture();

    dxContext->DrawIndexed(primitives.size()*dxIndexMultiplier,0,0);
}

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
    
    const std::vector<ShaderDX11::VERTEX_INPUT_ELEM>& vertexInputElems = ((ShaderDX11*)material->getShader())->getVertexInputElems();
    for (int i=0;i<vertices.size();i++) {
        SDL_Log("*****\n");
        int uvIndex = 0;
        for (int j=0;j<vertexInputElems.size();j++) {
            switch (vertexInputElems[j]) {
                case ShaderDX11::VERTEX_INPUT_ELEM::POSITION: {
                    dxVertexData.push_back(vertices[i].pos.x);
                    dxVertexData.push_back(vertices[i].pos.y);
                    dxVertexData.push_back(vertices[i].pos.z);
                    dxVertexData.push_back(1.f);
                    SDL_Log("POSITION\n");
                } break;
                case ShaderDX11::VERTEX_INPUT_ELEM::NORMAL: {
                    dxVertexData.push_back(vertices[i].normal.x);
                    dxVertexData.push_back(vertices[i].normal.y);
                    dxVertexData.push_back(vertices[i].normal.z);
                    SDL_Log("NORMAL\n");
                } break;
                case ShaderDX11::VERTEX_INPUT_ELEM::TEXCOORD: {
                    dxVertexData.push_back(vertices[i].uv[1-uvIndex].x);
                    dxVertexData.push_back(vertices[i].uv[1-uvIndex].y);
                    SDL_Log("TEXCOORD%d\n",uvIndex);
                    uvIndex++;
                } break;
                case ShaderDX11::VERTEX_INPUT_ELEM::COLOR: {
                    dxVertexData.push_back(vertices[i].color.red);
                    dxVertexData.push_back(vertices[i].color.green);
                    dxVertexData.push_back(vertices[i].color.blue);
                    dxVertexData.push_back(vertices[i].color.alpha);
                    SDL_Log("COLOR\n");
                }
            }
        }
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

    ((ShaderDX11*)material->getShader())->useVertexInputLayout();

    const std::vector<ShaderDX11::VERTEX_INPUT_ELEM>& vertexInputElems = ((ShaderDX11*)material->getShader())->getVertexInputElems();
    UINT stride = 0;
    //TODO: use dxgi format?
    for (int j=0;j<vertexInputElems.size();j++) {
        switch (vertexInputElems[j]) {
            case ShaderDX11::VERTEX_INPUT_ELEM::POSITION: {
                stride += sizeof(FLOAT) * 4;
            } break;
            case ShaderDX11::VERTEX_INPUT_ELEM::NORMAL: {
                stride += sizeof(FLOAT) * 3;
            } break;
            case ShaderDX11::VERTEX_INPUT_ELEM::TEXCOORD: {
                stride += sizeof(FLOAT) * 2;
            } break;
            case ShaderDX11::VERTEX_INPUT_ELEM::COLOR: {
                stride += sizeof(FLOAT) * 4;
            } break;
        }
    }
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
    ((GraphicsDX11*)graphics)->useMatrixCBuffer();
    shader->useSamplers();
    for (int i=0;i<material->getTextureCount();i++) {
        ((TextureDX11*)material->getTexture(i))->useTexture(i);
    }

    ((WindowDX11*)graphics->getWindow())->setZBufferWriteState(opaque);

    dxContext->DrawIndexed(primitives.size()*dxIndexMultiplier,0,0);
}

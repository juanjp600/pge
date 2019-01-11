#include <Shader/Shader.h>
#include "ShaderDX11.h"
#include <Window/Window.h>
#include "../Window/WindowDX11.h"
#include <fstream>

using namespace PGE;

Shader* Shader::load(Graphics* gfx, const String& path) {
    return new ShaderDX11(gfx,path);
}

ShaderDX11::ShaderDX11(Graphics* gfx,const String& path) {    
    graphics = gfx;

    filepath = path;

    char* buf = new char[512];

    std::ifstream vertexReflectionInfo; vertexReflectionInfo.open(String(path,"vertex.dxri").cstr(), std::ios_base::in | std::ios_base::binary);
    int inputParamCount = 0; vertexReflectionInfo.read((char*)(void*)&inputParamCount,1);
    for (int i=0;i<inputParamCount;i++) {
        String name = "";
        char chr; vertexReflectionInfo.read(&chr,1);
        while (chr!=0) {
            name = String(name,chr);
            vertexReflectionInfo.read(&chr, 1);
        }
        char* nameBuf = new char[name.size() + 1];
        memcpy(nameBuf,name.cstr(),(name.size()+1)*sizeof(char));
        int index = 0;
        vertexReflectionInfo.read((char*)(void*)&index,1);
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        vertexReflectionInfo.read((char*)(void*)&format,1);

        D3D11_INPUT_ELEMENT_DESC vertexInputElemDesc;

        vertexInputElemDesc.SemanticName = nameBuf;
        vertexInputElemDesc.SemanticIndex = index;
        vertexInputElemDesc.Format = format;
        vertexInputElemDesc.InputSlot = 0;
        vertexInputElemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        vertexInputElemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        vertexInputElemDesc.InstanceDataStepRate = 0;

        if (name.equals("POSITION")) {
            vertexInputElems.push_back(VERTEX_INPUT_ELEM::POSITION);
        } else if (name.equals("NORMAL")) {
            vertexInputElems.push_back(VERTEX_INPUT_ELEM::NORMAL);
        } else if (name.equals("TEXCOORD")) {
            vertexInputElems.push_back(VERTEX_INPUT_ELEM::TEXCOORD);
        } else if (name.equals("COLOR")) {
            vertexInputElems.push_back(VERTEX_INPUT_ELEM::COLOR);
        }
        
        dxVertexInputElemDesc.push_back(vertexInputElemDesc);
    }
    vertexReflectionInfo.close();

    vertexShaderBytecode.clear();
    std::ifstream vertexSourceFile; vertexSourceFile.open(String(path,"vertex.dxbc").cstr(), std::ios_base::in | std::ios_base::binary);
    SDL_Log("Shader file: %s %s",path.cstr(),String(path,"vertex.dxbc").cstr());
    while (!vertexSourceFile.eof()) {
        int writeInd = vertexShaderBytecode.size();
        vertexSourceFile.read(buf,512); int bytesRead = vertexSourceFile.gcount();
        if (bytesRead<=0) { break; }
        vertexShaderBytecode.resize(vertexShaderBytecode.size()+bytesRead);
        memcpy(&(vertexShaderBytecode[writeInd]),buf,bytesRead);
    }
    SDL_Log("Shader code size: %d",vertexShaderBytecode.size());
    vertexSourceFile.close();

    std::ifstream fragmentReflectionInfo; fragmentReflectionInfo.open(String(path, "fragment.dxri").cstr(), std::ios_base::in | std::ios_base::binary);
    int samplerCount = 0; fragmentReflectionInfo.read((char*)(void*)&samplerCount, 1);

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ID3D11Device* dxDevice = ((WindowDX11*)graphics->getWindow())->getDxDevice();
    for (int i=0;i<samplerCount;i++) {
        ID3D11SamplerState* samplerState = NULL;
        dxDevice->CreateSamplerState(&samplerDesc, &samplerState);
        dxSamplerState.push_back(samplerState);
    }

    fragmentReflectionInfo.close();

    fragmentShaderBytecode.clear();
    std::ifstream fragmentSourceFile; fragmentSourceFile.open(String(path,"fragment.dxbc").cstr(), std::ios_base::in | std::ios_base::binary);
    SDL_Log("Shader file: %s %s", path.cstr(), String(path, "fragment.dxbc").cstr());
    while (!fragmentSourceFile.eof()) {
        int writeInd = fragmentShaderBytecode.size();
        fragmentSourceFile.read(buf,512); int bytesRead = fragmentSourceFile.gcount();
        if (bytesRead<=0) { break; }
        fragmentShaderBytecode.resize(fragmentShaderBytecode.size()+bytesRead);
        memcpy(&(fragmentShaderBytecode[writeInd]),buf,bytesRead);
    }
    SDL_Log("Shader code size: %d", fragmentShaderBytecode.size());
    fragmentSourceFile.close();

    dxDevice->CreateVertexShader(vertexShaderBytecode.data(),sizeof(uint8_t)*vertexShaderBytecode.size(),NULL,&dxVertexShader);
    dxDevice->CreatePixelShader(fragmentShaderBytecode.data(),sizeof(uint8_t)*fragmentShaderBytecode.size(),NULL,&dxFragmentShader);

    dxDevice->CreateInputLayout(dxVertexInputElemDesc.data(), dxVertexInputElemDesc.size(), getDxVsCode(), getDxVsCodeLen() * sizeof(uint8_t), &dxVertexInputLayout);

    delete[] buf;
}

ShaderDX11::~ShaderDX11() {
    for (int i=0;i<dxVertexInputElemDesc.size();i++) {
        delete[] dxVertexInputElemDesc[i].SemanticName;
    }
    dxVertexInputElemDesc.clear();
    for (int i=0;i<dxSamplerState.size();i++) {
        dxSamplerState[i]->Release();
    }
    dxSamplerState.clear();

    dxVertexInputLayout->Release();
    dxVertexShader->Release();
    dxFragmentShader->Release();
}

uint8_t* ShaderDX11::getDxVsCode() {
    return vertexShaderBytecode.data();
}

int ShaderDX11::getDxVsCodeLen() const {
    return vertexShaderBytecode.size();
}

uint8_t* ShaderDX11::getDxFsCode() {
    return fragmentShaderBytecode.data();
}

int ShaderDX11::getDxFsCodeLen() const {
    return fragmentShaderBytecode.size();
}

const std::vector<ShaderDX11::VERTEX_INPUT_ELEM>& ShaderDX11::getVertexInputElems() const {
    return vertexInputElems;
}

void ShaderDX11::useShader() {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();
    dxContext->VSSetShader(dxVertexShader,NULL,0);
    dxContext->PSSetShader(dxFragmentShader,NULL,0);
}

void ShaderDX11::useVertexInputLayout() {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();
    dxContext->IASetInputLayout(dxVertexInputLayout);
}

void ShaderDX11::useSamplers() {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();
    dxContext->PSSetSamplers(0, dxSamplerState.size(), dxSamplerState.data());
}

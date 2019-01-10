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

    vertexShaderBytecode.clear();
    std::ifstream vertexSourceFile; vertexSourceFile.open(String(path,"vertex.dxbc").cstr());
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

    fragmentShaderBytecode.clear();
    std::ifstream fragmentSourceFile; fragmentSourceFile.open(String(path,"fragment.dxbc").cstr());
    while (!fragmentSourceFile.eof()) {
        int writeInd = fragmentShaderBytecode.size();
        fragmentSourceFile.read(buf,512); int bytesRead = fragmentSourceFile.gcount();
        if (bytesRead<=0) { break; }
        fragmentShaderBytecode.resize(fragmentShaderBytecode.size()+bytesRead);
        memcpy(&(fragmentShaderBytecode[writeInd]),buf,bytesRead);
    }
    fragmentSourceFile.close();

    ID3D11Device* dxDevice = ((WindowDX11*)graphics->getWindow())->getDxDevice();
    dxDevice->CreateVertexShader(vertexShaderBytecode.data(),sizeof(uint8_t)*vertexShaderBytecode.size(),NULL,&dxVertexShader);
    dxDevice->CreatePixelShader(fragmentShaderBytecode.data(),sizeof(uint8_t)*fragmentShaderBytecode.size(),NULL,&dxFragmentShader);

    delete[] buf;
}

ShaderDX11::~ShaderDX11() {
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

void ShaderDX11::useShader() {
    ID3D11DeviceContext* dxContext = ((WindowDX11*)graphics->getWindow())->getDxContext();
    dxContext->VSSetShader(dxVertexShader,NULL,0);
    dxContext->PSSetShader(dxFragmentShader,NULL,0);
}

#include <PGE/Graphics/Shader.h>
#include <PGE/Graphics/Graphics.h>
#include "ShaderDX11.h"
#include <PGE/Exception/Exception.h>
#include <fstream>
#include "../GraphicsDX11.h"

using namespace PGE;

ShaderDX11::ShaderDX11(Graphics* gfx,const FilePath& path) : resourceManager(3) {
    graphics = gfx;

    filepath = path;

    std::ifstream reflectionInfo((path.str() + "reflection.dxri").cstr(), std::ios::binary);

    readConstantBuffers(reflectionInfo,vertexConstantBuffers);

    int inputParamCount = 0; reflectionInfo.read((char*)(void*)&inputParamCount,1);
    std::vector<String> vertexInputElemSemanticNames(inputParamCount);
    std::vector<D3D11_INPUT_ELEMENT_DESC> dxVertexInputElemDesc(inputParamCount);
    for (int i=0;i<inputParamCount;i++) {
        String propertyName = "";
        char chr; reflectionInfo.read(&chr,1);
        while (chr!=0) {
            propertyName += chr;
            reflectionInfo.read(&chr, 1);
        }
        vertexInputElems.push_back(propertyName);

        String semanticName = "";
        reflectionInfo.read(&chr,1);
        while (chr!=0) {
            semanticName += chr;
            reflectionInfo.read(&chr, 1);
        }
        vertexInputElemSemanticNames[i] = semanticName;
        const char* nameBuf = vertexInputElemSemanticNames[i].cstr();
        int index = 0;
        reflectionInfo.read((char*)(void*)&index,1);
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        reflectionInfo.read((char*)(void*)&format,1);

        D3D11_INPUT_ELEMENT_DESC vertexInputElemDesc;

        vertexInputElemDesc.SemanticName = nameBuf;
        vertexInputElemDesc.SemanticIndex = index;
        vertexInputElemDesc.Format = format;
        vertexInputElemDesc.InputSlot = 0;
        vertexInputElemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        vertexInputElemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        vertexInputElemDesc.InstanceDataStepRate = 0;

        dxVertexInputElemDesc[i] = vertexInputElemDesc;
    }

    readConstantBuffers(reflectionInfo, fragmentConstantBuffers);

    int samplerCount = 0; reflectionInfo.read((char*)(void*)&samplerCount, 1);

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.MaxAnisotropy = 8;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MipLODBias = -0.1f;

    ID3D11Device* dxDevice = ((GraphicsDX11*)graphics)->getDxDevice();
    resourceManager.increaseSize(samplerCount);
    dxSamplerState = ResourceViewVector<ID3D11SamplerState*>::withSize(samplerCount);
    for (int i = 0; i < samplerCount; i++) {
        dxSamplerState[i] = resourceManager.addNewResource<D3D11SamplerState>(dxDevice, samplerDesc);
    }

    reflectionInfo.close();

    std::vector<byte> vertexShaderBytecode;
    (path + "vertex.dxbc").readBytes(vertexShaderBytecode);
    PGE_ASSERT(vertexShaderBytecode.size() > 0, "Vertex shader is empty (filename: " + path.str() + ")");

    std::vector<byte> fragmentShaderBytecode;
    (path + "fragment.dxbc").readBytes(fragmentShaderBytecode);
    PGE_ASSERT(fragmentShaderBytecode.size() > 0, "Fragment shader is empty (filename: " + path.str() + ")");

    dxVertexShader = resourceManager.addNewResource<D3D11VertexShader>(dxDevice, vertexShaderBytecode);
    dxFragmentShader = resourceManager.addNewResource<D3D11PixelShader>(dxDevice, fragmentShaderBytecode);
    dxVertexInputLayout = resourceManager.addNewResource<D3D11InputLayout>(dxDevice, dxVertexInputElemDesc, vertexShaderBytecode);
}

void ShaderDX11::readConstantBuffers(std::ifstream& reflectionInfo, ResourceViewVector<CBufferInfo*>& constantBuffers) {
    int cBufferCount = 0; reflectionInfo.read((char*)(void*)&cBufferCount, 1);
    resourceManager.increaseSize(cBufferCount * 2);
    for (int i = 0; i < cBufferCount; i++) {
        String cBufferName = "";
        char chr; reflectionInfo.read(&chr, 1);
        while (chr != 0) {
            cBufferName += chr;
            reflectionInfo.read(&chr, 1);
        }
        int cBufferSize = 0; reflectionInfo.read((char*)(void*)&cBufferSize, 1);
        
        CBufferInfoRef constantBuffer = resourceManager.addNewResource<CBufferInfoOwner>(graphics, cBufferName, cBufferSize, &resourceManager);
        constantBuffers.add(constantBuffer);

        int varCount = 0; reflectionInfo.read((char*)(void*)&varCount, 1);
        for (int j = 0; j < varCount; j++) {
            String varName = "";
            reflectionInfo.read(&chr, 1);
            while (chr != 0) {
                varName += chr;
                reflectionInfo.read(&chr, 1);
            }
            int varOffset = 0; reflectionInfo.read((char*)(void*)&varOffset, 1);
            int varSize = 0; reflectionInfo.read((char*)(void*)&varSize, 1);
            constantBuffer->addConstant(varName, ConstantDX11(constantBuffer,varOffset,varSize));
        }
    }
}

Shader::Constant* ShaderDX11::getVertexShaderConstant(const String& name) {
    for (auto cBuffer : vertexConstantBuffers) {
        auto map = cBuffer->getConstants();
        auto it = map->find(name);
        if (it != map->end()) {
            return &it->second;
        }
    }
    return nullptr;
}

Shader::Constant* ShaderDX11::getFragmentShaderConstant(const String& name) {
    for (auto cBuffer : fragmentConstantBuffers) {
        auto map = cBuffer->getConstants();
        auto it = map->find(name);
        if (it != map->end()) {
            return &it->second;
        }
    }
    return nullptr;
}

const std::vector<String>& ShaderDX11::getVertexInputElems() const {
    return vertexInputElems;
}

void ShaderDX11::useShader() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    for (int i = 0; i < (int)vertexConstantBuffers.size(); i++) {
        vertexConstantBuffers[i]->update();
        dxContext->VSSetConstantBuffers(i,1,&vertexConstantBuffers[i]->getDxCBuffer());
    }

    for (int i = 0; i < (int)fragmentConstantBuffers.size(); i++) {
        fragmentConstantBuffers[i]->update();
        dxContext->PSSetConstantBuffers(i,1,&fragmentConstantBuffers[i]->getDxCBuffer());
    }
    
    dxContext->VSSetShader(dxVertexShader,NULL,0);
    dxContext->PSSetShader(dxFragmentShader,NULL,0);
}

void ShaderDX11::useVertexInputLayout() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    dxContext->IASetInputLayout(dxVertexInputLayout);
}

void ShaderDX11::useSamplers() {
    ID3D11DeviceContext* dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    dxContext->PSSetSamplers(0, (UINT)dxSamplerState.size(), dxSamplerState.data());
}

ShaderDX11::CBufferInfoOwner::CBufferInfoOwner(Graphics* gfx, const String& nm, int sz, ResourceManager* rm) {
    resource = new ShaderDX11::CBufferInfo(gfx, nm, sz, rm);
}

ShaderDX11::CBufferInfo::CBufferInfo(Graphics* graphics, const String& nm, int sz, ResourceManager* resourceManager) {
    name = nm;
    size = sz;
    int cBufferSize = size;
    //round up to a multiple of 16, see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc
    if ((cBufferSize % 16) != 0) { cBufferSize += 16 - (cBufferSize % 16); }
    data = new byte[cBufferSize];

    D3D11_BUFFER_DESC cBufferDesc;
    D3D11_SUBRESOURCE_DATA cBufferSubresourceData;

    ZeroMemory( &cBufferDesc, sizeof(D3D11_BUFFER_DESC) );
    cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    cBufferDesc.ByteWidth = cBufferSize;
    cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cBufferDesc.CPUAccessFlags = 0;

    ZeroMemory( &cBufferSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA) );
    cBufferSubresourceData.pSysMem = data;

    HRESULT hResult = 0;

    dxCBuffer = resourceManager->addNewResource<D3D11Buffer>(((GraphicsDX11*)graphics)->getDxDevice(), cBufferDesc, cBufferSubresourceData);

    dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    dirty = true;
}

ShaderDX11::CBufferInfo::~CBufferInfo() {
    delete[] data;
}

byte* ShaderDX11::CBufferInfo::getData() {
    return data;
}

std::unordered_map<String::Key, ShaderDX11::ConstantDX11>* ShaderDX11::CBufferInfo::getConstants() {
    return &constants;
}

void ShaderDX11::CBufferInfo::addConstant(const String& name, const ShaderDX11::ConstantDX11& constant) {
    constants.emplace(name, constant);
}

bool ShaderDX11::CBufferInfo::isDirty() const {
    return dirty;
}

void ShaderDX11::CBufferInfo::markAsDirty() {
    dirty = true;
}

void ShaderDX11::CBufferInfo::update() {
    if (!dirty) { return; }
    dxContext->UpdateSubresource(dxCBuffer,0,NULL,data,0,0);
    dirty = false;
}

D3D11Buffer::View ShaderDX11::CBufferInfo::getDxCBuffer() {
    return dxCBuffer;
}

ShaderDX11::ConstantDX11::ConstantDX11(ShaderDX11::CBufferInfoRef cBuffer, int offst, int sz) {
    constantBuffer = cBuffer;
    offset = offst;
    size = sz;
}

void ShaderDX11::ConstantDX11::setValue(const Matrix4x4f& value) {
    memcpy(constantBuffer->getData()+offset,value.transpose()[0],16*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Vector2f& value) {
    float arr[2]; arr[0] = value.x; arr[1] = value.y;
    memcpy(constantBuffer->getData() + offset, arr, 2 * sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Vector3f& value) {
    float arr[3]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z;
    memcpy(constantBuffer->getData() + offset, arr, 3 * sizeof(float));
    if (size == 4 * sizeof(float)) {
        float one = 1.f;
        memcpy(constantBuffer->getData() + offset + (3 * sizeof(float)), &one, sizeof(float));
    }
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Vector4f& value) {
    float arr[4]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z; arr[3] = value.w;
    memcpy(constantBuffer->getData()+offset,arr,4*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Color& value) {
    float arr[4]; arr[0] = value.red; arr[1] = value.green; arr[2] = value.blue; arr[3] = value.alpha;
    memcpy(constantBuffer->getData()+offset,arr,4*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(float value) {
    memcpy(constantBuffer->getData()+offset,&value,sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(int value) {
    u32 valUi32 = value;
    memcpy(constantBuffer->getData()+offset,&valUi32,sizeof(u32));
    constantBuffer->markAsDirty();
}


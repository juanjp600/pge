#include <Shader/Shader.h>
#include <Graphics/Graphics.h>
#include "ShaderDX11.h"
#include <Exception/Exception.h>
#include <fstream>
#include <Misc/FileUtil.h>
#include "../Graphics/GraphicsDX11.h"

using namespace PGE;

ShaderDX11::ShaderDX11(Graphics* gfx,const FilePath& path) {
    graphics = gfx;

    filepath = path;

    std::ifstream reflectionInfo; reflectionInfo.open(String(path.str(),"reflection.dxri").cstr(), std::ios_base::in | std::ios_base::binary);

    readConstantBuffers(reflectionInfo,vertexConstantBuffers);

    int inputParamCount = 0; reflectionInfo.read((char*)(void*)&inputParamCount,1);
    vertexInputElemSemanticNames.resize(inputParamCount);
    for (int i=0;i<inputParamCount;i++) {
        String propertyName = "";
        char chr; reflectionInfo.read(&chr,1);
        while (chr!=0) {
            propertyName = String(propertyName,chr);
            reflectionInfo.read(&chr, 1);
        }
        vertexInputElems.push_back(propertyName);

        String semanticName = "";
        reflectionInfo.read(&chr,1);
        while (chr!=0) {
            semanticName = String(semanticName,chr);
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

        dxVertexInputElemDesc.push_back(vertexInputElemDesc);
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

    D3D11DeviceRef dxDevice = ((GraphicsDX11*)graphics)->getDxDevice();
    dxSamplerState = ResourceRefVector<ID3D11SamplerState*>::withSize(samplerCount);
    for (int i = 0; i < samplerCount; i++) {
        dxSamplerState[i] = D3D11SamplerStateOwner::createRef(dxDevice, samplerDesc, resourceManager);
    }

    reflectionInfo.close();

    vertexShaderBytecode = FileUtil::readBytes(path + "vertex.dxbc");
    if (vertexShaderBytecode.size() <= 0) {
        throw Exception("ShaderDX11","Vertex shader is empty (filename: "+path.str() +")");
    }

    fragmentShaderBytecode = FileUtil::readBytes(path + "fragment.dxbc");
    if (fragmentShaderBytecode.size() <= 0) {
        throw Exception("ShaderDX11","Fragment shader is empty (filename: "+path.str()+")");
    }

    dxVertexShader = D3D11VertexShaderOwner::createRef(dxDevice, vertexShaderBytecode, resourceManager);
    dxFragmentShader = D3D11PixelShaderOwner::createRef(dxDevice, fragmentShaderBytecode, resourceManager);
    dxVertexInputLayout = D3D11InputLayoutOwner::createRef(dxDevice, dxVertexInputElemDesc, vertexShaderBytecode, resourceManager);
}

void ShaderDX11::readConstantBuffers(std::ifstream& reflectionInfo, ResourceRefVector<CBufferInfo*>& constantBuffers) {
    int cBufferCount = 0; reflectionInfo.read((char*)(void*)&cBufferCount, 1);
    for (int i = 0; i < cBufferCount; i++) {
        String cBufferName = "";
        char chr; reflectionInfo.read(&chr, 1);
        while (chr != 0) {
            cBufferName = String(cBufferName, chr);
            reflectionInfo.read(&chr, 1);
        }
        int cBufferSize = 0; reflectionInfo.read((char*)(void*)&cBufferSize, 1);
        
        CBufferInfoRef constantBuffer = CBufferInfoOwner::createRef(graphics,cBufferName,cBufferSize,resourceManager);
        constantBuffers.add(constantBuffer);


        int varCount = 0; reflectionInfo.read((char*)(void*)&varCount, 1);
        for (int j = 0; j < varCount; j++) {
            String varName = "";
            reflectionInfo.read(&chr, 1);
            while (chr != 0) {
                varName = String(varName, chr);
                reflectionInfo.read(&chr, 1);
            }
            int varOffset = 0; reflectionInfo.read((char*)(void*)&varOffset, 1);
            int varSize = 0; reflectionInfo.read((char*)(void*)&varSize, 1);
            constantBuffer->addConstant(ConstantDX11(constantBuffer,varName,varOffset,varSize));
        }
    }
}

Shader::Constant* ShaderDX11::getVertexShaderConstant(String name) {
    for (int i = 0; i < (int)vertexConstantBuffers.size(); i++) {
        std::vector<ConstantDX11>& vars = vertexConstantBuffers[i]->getConstants(); 
        for (int j = 0; j < (int)vars.size(); j++) {
            if (name.equals(vars[j].getName())) {
                return &vars[j];
            }
        }
    }
    return nullptr;
}

Shader::Constant* ShaderDX11::getFragmentShaderConstant(String name) {
    for (int i = 0; i < (int)fragmentConstantBuffers.size(); i++) {
        std::vector<ConstantDX11>& vars = fragmentConstantBuffers[i]->getConstants(); 
        for (int j = 0; j < (int)vars.size(); j++) {
            if (name.equals(vars[j].getName())) {
                return &vars[j];
            }
        }
    }
    return nullptr;
}

uint8_t* ShaderDX11::getDxVsCode() {
    return vertexShaderBytecode.data();
}

int ShaderDX11::getDxVsCodeLen() const {
    return (int)vertexShaderBytecode.size();
}

uint8_t* ShaderDX11::getDxFsCode() {
    return fragmentShaderBytecode.data();
}

int ShaderDX11::getDxFsCodeLen() const {
    return (int)fragmentShaderBytecode.size();
}

const std::vector<String>& ShaderDX11::getVertexInputElems() const {
    return vertexInputElems;
}

void ShaderDX11::useShader() {
    D3D11DeviceContextRef dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    for (int i = 0; i < (int)vertexConstantBuffers.size(); i++) {
        vertexConstantBuffers[i].get()->update();
        ID3D11Buffer* dxCBuffers[] = { vertexConstantBuffers[i]->getDxCBuffer().get() };
        dxContext->VSSetConstantBuffers(i,1,dxCBuffers);
    }

    for (int i = 0; i < (int)fragmentConstantBuffers.size(); i++) {
        fragmentConstantBuffers[i].get()->update();
        ID3D11Buffer* dxCBuffers[] = { fragmentConstantBuffers[i]->getDxCBuffer().get() };
        dxContext->PSSetConstantBuffers(i,1,dxCBuffers);
    }
    
    dxContext->VSSetShader(dxVertexShader.get(),NULL,0);
    dxContext->PSSetShader(dxFragmentShader.get(),NULL,0);
}

void ShaderDX11::useVertexInputLayout() {
    D3D11DeviceContextRef dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    dxContext->IASetInputLayout(dxVertexInputLayout.get());
}

void ShaderDX11::useSamplers() {
    D3D11DeviceContextRef dxContext = ((GraphicsDX11*)graphics)->getDxContext();
    dxContext->PSSetSamplers(0, (UINT)dxSamplerState.size(), dxSamplerState.data());
}

ShaderDX11::CBufferInfoOwner::CBufferInfoOwner(Graphics* gfx, String nm, int sz, ResourceManager& rm) {
    graphics = gfx; name = nm; size = sz; resourceManager = &rm;
}

void ShaderDX11::CBufferInfoOwner::initInternal() {
    resource = new ShaderDX11::CBufferInfo(graphics, name, size, *resourceManager);
}

ShaderDX11::CBufferInfoRef ShaderDX11::CBufferInfoOwner::createRef(Graphics* gfx, String nm, int sz, ResourceManager& rm) {
    ShaderDX11::CBufferInfoOwner* owner = new ShaderDX11::CBufferInfoOwner(gfx, nm, sz, rm);
    rm.addResource(owner);
    owner->init();
    return ShaderDX11::CBufferInfoRef(owner->get());
}

ShaderDX11::CBufferInfo::CBufferInfo() { }

ShaderDX11::CBufferInfo::CBufferInfo(Graphics* graphics, String nm, int sz, ResourceManager& resourceManager) {
    name = nm;
    size = sz;
    data = new uint8_t[size];

    D3D11_BUFFER_DESC cBufferDesc;
    D3D11_SUBRESOURCE_DATA cBufferSubresourceData;

    ZeroMemory( &cBufferDesc, sizeof(D3D11_BUFFER_DESC) );
    cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    cBufferDesc.ByteWidth = sizeof(FLOAT)*48;
    cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cBufferDesc.CPUAccessFlags = 0;

    ZeroMemory( &cBufferSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA) );
    cBufferSubresourceData.pSysMem = data;

    HRESULT hResult = 0;

    dxCBuffer = D3D11BufferOwner::createRef(((GraphicsDX11*)graphics)->getDxDevice(), cBufferDesc, cBufferSubresourceData, resourceManager);

    dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    dirty = true;
}

ShaderDX11::CBufferInfo::~CBufferInfo() {
    delete[] data;
}

uint8_t* ShaderDX11::CBufferInfo::getData() {
    return data;
}

std::vector<ShaderDX11::ConstantDX11>& ShaderDX11::CBufferInfo::getConstants() {
    return constants;
}

void ShaderDX11::CBufferInfo::addConstant(ShaderDX11::ConstantDX11 constant) {
    constants.push_back(constant);
}

bool ShaderDX11::CBufferInfo::isDirty() const {
    return dirty;
}

void ShaderDX11::CBufferInfo::markAsDirty() {
    dirty = true;
}

void ShaderDX11::CBufferInfo::update() {
    if (!dirty) { return; }
    dxContext->UpdateSubresource(dxCBuffer.get(),0,NULL,data,0,0);
    dirty = false;
}

D3D11BufferRef ShaderDX11::CBufferInfo::getDxCBuffer() {
    return D3D11BufferRef(dxCBuffer.get());
}

ShaderDX11::ConstantDX11::ConstantDX11(ShaderDX11::CBufferInfoRef cBuffer, String nm, int offst, int sz) {
    constantBuffer = cBuffer;
    name = nm;
    offset = offst;
    size = sz;
}

void ShaderDX11::ConstantDX11::setValue(Matrix4x4f value) {
    memcpy(constantBuffer->getData()+offset,value.transpose().elements,16*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(Vector2f value) {
    float arr[2]; arr[0] = value.x; arr[1] = value.y;
    memcpy(constantBuffer->getData() + offset, arr, 2 * sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(Vector3f value) {
    float arr[3]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z;
    memcpy(constantBuffer->getData() + offset, arr, 3 * sizeof(float));
    if (size == 4 * sizeof(float)) {
        float one = 1.f;
        memcpy(constantBuffer->getData() + offset + (3 * sizeof(float)), &one, sizeof(float));
    }
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(Vector4f value) {
    float arr[4]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z; arr[3] = value.w;
    memcpy(constantBuffer->getData()+offset,arr,4*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(Color value) {
    float arr[4]; arr[0] = value.red; arr[1] = value.green; arr[2] = value.blue; arr[3] = value.alpha;
    memcpy(constantBuffer->getData()+offset,arr,4*sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(float value) {
    memcpy(constantBuffer->getData()+offset,&value,sizeof(float));
    constantBuffer->markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(int value) {
    uint32_t valUi32 = value;
    memcpy(constantBuffer->getData()+offset,&valUi32,sizeof(uint32_t));
    constantBuffer->markAsDirty();
}

String ShaderDX11::ConstantDX11::getName() const {
    return name;
}


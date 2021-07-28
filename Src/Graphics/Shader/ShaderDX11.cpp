#include "../GraphicsDX11.h"

using namespace PGE;

ShaderDX11::ShaderDX11(Graphics* gfx,const FilePath& path) {
    graphics = gfx;

    filepath = path;

    BinaryReader reader(path + "reflection.dxri");

    readConstantBuffers(reader, vertexConstantBuffers);

    u32 propertyCount = reader.readUInt32();
    vertexInputElems.resize(propertyCount);
    // We have to keep the names in memory.
    std::vector<String> semanticNames(propertyCount);
    std::vector<D3D11_INPUT_ELEMENT_DESC> dxVertexInputElemDesc(propertyCount);
    for (int i = 0; i < (int)propertyCount; i++) {
        vertexInputElems[i] = reader.readNullTerminatedString();

        semanticNames[i] = reader.readNullTerminatedString();
        byte semanticIndex = reader.readByte();
        DXGI_FORMAT format = (DXGI_FORMAT)reader.readByte();

        D3D11_INPUT_ELEMENT_DESC vertexInputElemDesc;
        vertexInputElemDesc.SemanticName = semanticNames[i].cstr();
        vertexInputElemDesc.SemanticIndex = semanticIndex;
        vertexInputElemDesc.Format = format;
        vertexInputElemDesc.InputSlot = 0;
        vertexInputElemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        vertexInputElemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        vertexInputElemDesc.InstanceDataStepRate = 0;

        dxVertexInputElemDesc[i] = vertexInputElemDesc;
    }

    readConstantBuffers(reader, fragmentConstantBuffers);

    u32 samplerCount = reader.readUInt32();

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
    dxSamplerState = ResourceViewVector<ID3D11SamplerState*>::withSize(samplerCount);
    for (int i = 0; i < (int)samplerCount; i++) {
        dxSamplerState[i] = resourceManager.addNewResource<D3D11SamplerState>(dxDevice, samplerDesc);
    }

    std::vector<byte> vertexShaderBytecode = (path + "vertex.dxbc").readBytes();
    PGE_ASSERT(vertexShaderBytecode.size() > 0, "Vertex shader is empty (filename: " + path.str() + ")");

    std::vector<byte> fragmentShaderBytecode = (path + "fragment.dxbc").readBytes();
    PGE_ASSERT(fragmentShaderBytecode.size() > 0, "Fragment shader is empty (filename: " + path.str() + ")");

    dxVertexShader = resourceManager.addNewResource<D3D11VertexShader>(dxDevice, vertexShaderBytecode);
    dxFragmentShader = resourceManager.addNewResource<D3D11PixelShader>(dxDevice, fragmentShaderBytecode);
    dxVertexInputLayout = resourceManager.addNewResource<D3D11InputLayout>(dxDevice, dxVertexInputElemDesc, vertexShaderBytecode);
}

void ShaderDX11::readConstantBuffers(BinaryReader& reader, std::vector<CBufferInfo>& constantBuffers) {
    u32 cBufferCount = reader.readUInt32();

    for (int i = 0; i < (int)cBufferCount; i++) {
        String bufferName = reader.readNullTerminatedString();
        u32 cBufferSize = reader.readUInt32();
        CBufferInfo& constantBuffer = constantBuffers.emplace_back(graphics, bufferName, cBufferSize, resourceManager);

        String varName;
        u32 varCount = reader.readUInt32();
        for (int j = 0; j < (int)varCount; j++) {
            varName = reader.readNullTerminatedString();
            u32 varOffset = reader.readUInt32();
            u32 varSize = reader.readUInt32();
            constantBuffer.addConstant(varName, ConstantDX11(constantBuffer, varOffset, varSize));
        }
    }
}

Shader::Constant* ShaderDX11::getVertexShaderConstant(const String& name) {
    for (CBufferInfo& cBuffer : vertexConstantBuffers) {
        auto& map = cBuffer.getConstants();
        auto it = map.find(name);
        if (it != map.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

Shader::Constant* ShaderDX11::getFragmentShaderConstant(const String& name) {
    for (CBufferInfo& cBuffer : fragmentConstantBuffers) {
        auto& map = cBuffer.getConstants();
        auto it = map.find(name);
        if (it != map.end()) {
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
        vertexConstantBuffers[i].update();
        dxContext->VSSetConstantBuffers(i,1,&vertexConstantBuffers[i].getDxCBuffer());
    }

    for (int i = 0; i < (int)fragmentConstantBuffers.size(); i++) {
        fragmentConstantBuffers[i].update();
        dxContext->PSSetConstantBuffers(i,1,&fragmentConstantBuffers[i].getDxCBuffer());
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

ShaderDX11::CBufferInfo::CBufferInfo(Graphics* graphics, const String& nm, int sz, ResourceManager& resourceManager) {
    name = nm;
    size = sz;
    int cBufferSize = size;
    // Round up to a multiple of 16, see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_buffer_desc
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

    dxCBuffer = resourceManager.addNewResource<D3D11Buffer>(((GraphicsDX11*)graphics)->getDxDevice(), cBufferDesc, cBufferSubresourceData);

    dxContext = ((GraphicsDX11*)graphics)->getDxContext();

    dirty = true;
}

ShaderDX11::CBufferInfo::CBufferInfo(CBufferInfo&& other) noexcept {
    *this = std::move(other);
}

ShaderDX11::CBufferInfo& ShaderDX11::CBufferInfo::operator=(CBufferInfo&& other) noexcept {
    name = other.name;
    size = other.size;
    constants = std::move(other.constants);
    dxContext = other.dxContext;
    dxCBuffer = other.dxCBuffer;
    dirty = other.dirty;

    data = other.data;
    other.data = nullptr; // Clear other's data as we take ownership of it here.

    return *this;
}

ShaderDX11::CBufferInfo::~CBufferInfo() {
    if (data != nullptr) { delete[] data; }
}

byte* ShaderDX11::CBufferInfo::getData() {
    return data;
}

std::unordered_map<String::Key, ShaderDX11::ConstantDX11>& ShaderDX11::CBufferInfo::getConstants() {
    return constants;
}

void ShaderDX11::CBufferInfo::addConstant(const String& cName, const ShaderDX11::ConstantDX11& constant) {
    constants.emplace(cName, constant);
}

bool ShaderDX11::CBufferInfo::isDirty() const {
    return dirty;
}

void ShaderDX11::CBufferInfo::markAsDirty() {
    dirty = true;
}

void ShaderDX11::CBufferInfo::update() {
    if (!dirty) { return; }
    dxContext->UpdateSubresource(dxCBuffer,0,NULL,getData(),0,0);
    dirty = false;
}

D3D11Buffer::View ShaderDX11::CBufferInfo::getDxCBuffer() {
    return dxCBuffer;
}

ShaderDX11::ConstantDX11::ConstantDX11(ShaderDX11::CBufferInfo& cBuffer, int offst, int sz)
    : constantBuffer(cBuffer) {
    offset = offst;
    size = sz;
}

void ShaderDX11::ConstantDX11::setValue(const Matrix4x4f& value) {
    memcpy(constantBuffer.getData()+offset,value.transpose()[0],16*sizeof(float));
    constantBuffer.markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Vector2f& value) {
    float arr[2]; arr[0] = value.x; arr[1] = value.y;
    memcpy(constantBuffer.getData() + offset, arr, 2 * sizeof(float));
    constantBuffer.markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Vector3f& value) {
    float arr[3]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z;
    memcpy(constantBuffer.getData() + offset, arr, 3 * sizeof(float));
    if (size == 4 * sizeof(float)) {
        float one = 1.f;
        memcpy(constantBuffer.getData() + offset + (3 * sizeof(float)), &one, sizeof(float));
    }
    constantBuffer.markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Vector4f& value) {
    float arr[4]; arr[0] = value.x; arr[1] = value.y; arr[2] = value.z; arr[3] = value.w;
    memcpy(constantBuffer.getData()+offset,arr,4*sizeof(float));
    constantBuffer.markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(const Color& value) {
    float arr[4]; arr[0] = value.red; arr[1] = value.green; arr[2] = value.blue; arr[3] = value.alpha;
    memcpy(constantBuffer.getData()+offset,arr,4*sizeof(float));
    constantBuffer.markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(float value) {
    memcpy(constantBuffer.getData()+offset,&value,sizeof(float));
    constantBuffer.markAsDirty();
}

void ShaderDX11::ConstantDX11::setValue(int value) {
    u32 valUi32 = value;
    memcpy(constantBuffer.getData()+offset,&valUi32,sizeof(u32));
    constantBuffer.markAsDirty();
}


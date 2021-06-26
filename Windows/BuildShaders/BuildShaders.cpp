#include <d3d11.h>
#include <d3dcompiler.h>

#include <iostream>
#include <execution>
#include <filesystem>

#include <PGE/Exception/Exception.h>
#include <PGE/File/BinaryWriter.h>
#include <PGE/String/Key.h>

using namespace PGE;

static DXGI_FORMAT computeDxgiFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc) {
    // https://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
    if (paramDesc.Mask == 1) {
        switch (paramDesc.ComponentType) {
            case D3D_REGISTER_COMPONENT_UINT32: { return DXGI_FORMAT_R32_UINT; }
            case D3D_REGISTER_COMPONENT_SINT32: { return DXGI_FORMAT_R32_SINT; }
            case D3D_REGISTER_COMPONENT_FLOAT32: { return DXGI_FORMAT_R32_FLOAT; }
        }
    } else if (paramDesc.Mask <= 3) {
        switch (paramDesc.ComponentType) {
            case D3D_REGISTER_COMPONENT_UINT32: { return DXGI_FORMAT_R32G32_UINT; }
            case D3D_REGISTER_COMPONENT_SINT32: { return DXGI_FORMAT_R32G32_SINT; }
            case D3D_REGISTER_COMPONENT_FLOAT32: { return DXGI_FORMAT_R32G32_FLOAT; }
        }
    } else if (paramDesc.Mask <= 7) {
        switch (paramDesc.ComponentType) {
            case D3D_REGISTER_COMPONENT_UINT32: { return DXGI_FORMAT_R32G32B32_UINT; }
            case D3D_REGISTER_COMPONENT_SINT32: { return DXGI_FORMAT_R32G32B32_SINT; }
            case D3D_REGISTER_COMPONENT_FLOAT32: { return DXGI_FORMAT_R32G32B32_FLOAT; }
        }
    } else if (paramDesc.Mask <= 15) {
        switch (paramDesc.ComponentType) {
            case D3D_REGISTER_COMPONENT_UINT32: { return DXGI_FORMAT_R32G32B32A32_UINT; }
            case D3D_REGISTER_COMPONENT_SINT32: { return DXGI_FORMAT_R32G32B32A32_SINT; }
            case D3D_REGISTER_COMPONENT_FLOAT32: { return DXGI_FORMAT_R32G32B32A32_FLOAT; }
        }
    }
    throw PGE_CREATE_EX("Invalid DXGI format! (" + String::fromInt(paramDesc.Mask) + ", " + String::fromInt(paramDesc.ComponentType) + ")");
}

static u64 combineStringUInt(const String& str, u64 u) {
    return std::hash<String::Key>()(String::Key(str)) + u * 3;
}

class ReflectionInfo {
    public:
        ReflectionInfo(ID3DBlob* shader) {
            HRESULT hr = D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection);
            PGE_ASSERT(!FAILED(hr), "Epic reflection fail " + String::fromInt(hr));
        }

        ReflectionInfo(const ReflectionInfo& other) {
            reflection = other.reflection;
            reflection->AddRef();
        }

        ReflectionInfo& operator=(const ReflectionInfo& other) {
            reflection = other.reflection;
            reflection->AddRef();
        }

        ~ReflectionInfo() {
            reflection->Release();
        }

        operator ID3D11ShaderReflection*() {
            return reflection;
        }

        ID3D11ShaderReflection* operator->() {
            return reflection;
        }

    private:
        ID3D11ShaderReflection* reflection;
};

static void writeConstants(BinaryWriter& writer, const String& cBufferName, ReflectionInfo info) {
    ID3D11ShaderReflectionConstantBuffer* cBuffer = info->GetConstantBufferByIndex(0);
    D3D11_SHADER_BUFFER_DESC cBufferDesc;
    cBuffer->GetDesc(&cBufferDesc);
    PGE_ASSERT(String(cBufferDesc.Name) == cBufferName, "Wrong constant name! (" + String(cBufferDesc.Name) + ")");

    writer.writeUInt(cBufferDesc.Size);
    writer.writeUInt(cBufferDesc.Variables);
    for (unsigned i = 0; i < cBufferDesc.Variables; ++i) {
        ID3D11ShaderReflectionVariable* cBufferVar = cBuffer->GetVariableByIndex(i);
        D3D11_SHADER_VARIABLE_DESC cBufferVarDesc;
        cBufferVar->GetDesc(&cBufferVarDesc);
        writer.writeNullTerminatedString(cBufferVarDesc.Name);
        writer.writeUInt(cBufferVarDesc.StartOffset);
        writer.writeUInt(cBufferVarDesc.Size);
    }
}

static std::unordered_map<u64, String> parseVertexInput(const String& input) {
    // struct VS_INPUT {
    //     'TYPE' 'INPUT_NAME' : 'SEMANTIC_NAME''SEMANTIC_INDEX';
    //     'TYPE' 'INPUT_NAME' : 'SEMANTIC_NAME';
    //     'TYPE' 'INPUT_NAME' : 'SEMANTIC_NAME''SEMANTIC_INDEX';
    // }

    std::unordered_map<u64, String> inputNameSemanticRelation;

    String vsInput = "struct VS_INPUT";
    String::Iterator before = input.findFirst(vsInput) + vsInput.length();
    // Space before {
    while (isspace(*++before));
    // {
    PGE_ASSERT(*before == '{', "Expected \"{\", found \"" + *before + '"');
    ++before;
    // Whitespace before first type
    while (isspace(*++before));
    while (*before != '}') {
        // Type skip
        while (!isspace(*++before));
        // Whitespace after type
        while (isspace(*++before));
        
        // Type
        String::Iterator after = before;
        while (!isspace(*++after));
        String inputName = input.substr(before, after);

        before = after;
        // Whitespace before colon
        while (isspace(*++before));
        PGE_ASSERT(*before == ':', "Expected \":\", found \"" + *before + '"');
        ++before;
        // Whitespace after colon
        while (isspace(*++before));

        after = before;
        while (!isdigit(*++after) && *after != ';');

        String semanticName = input.substr(before, after);
        byte semanticIndex;
        if (*after == ';') {
            semanticIndex = 0;
        } else {
            semanticIndex = *after - '0';
        }
        inputNameSemanticRelation.emplace(combineStringUInt(semanticName, semanticIndex), inputName);

        before = after;
        // Skip ;
        ++before;
        // Whitespace before type
        while (isspace(*++before));
    }

    return inputNameSemanticRelation;
}

static void compileDX11Reflection(const FilePath& path, const String& input, ID3DBlob* vsBlob, ID3DBlob* psBlob) {
    // Parsing Vertex input.
    std::unordered_map<u64, String> inputNameSemanticRelation = parseVertexInput(input);

    BinaryWriter writer(path);

    D3D11_SHADER_DESC desc;

    ReflectionInfo vsInfo(vsBlob);
    vsInfo->GetDesc(&desc);
    if (desc.ConstantBuffers == 1) {
        writeConstants(writer, "cbVertex", vsInfo);
    } else if (desc.ConstantBuffers == 0) {
        writer.writeUInt(0);
    } else {
        throw PGE_CREATE_EX("Too many vertex constant buffers!");
    }

    writer.writeUInt(desc.InputParameters);
    for (UINT i = 0; i < desc.InputParameters; ++i) {
        D3D11_SIGNATURE_PARAMETER_DESC vsParamDesc;
        vsInfo->GetInputParameterDesc(i, &vsParamDesc);

        const auto& it = inputNameSemanticRelation.find(combineStringUInt(vsParamDesc.SemanticName, vsParamDesc.SemanticIndex));
        PGE_ASSERT(it != inputNameSemanticRelation.end(), "Couldn't find semantic (" + String(vsParamDesc.SemanticName) + String::fromInt(vsParamDesc.SemanticIndex) + ")");
        writer.writeNullTerminatedString(it->second);
        writer.writeNullTerminatedString(vsParamDesc.SemanticName);
        writer.writeByte(vsParamDesc.SemanticIndex);

        writer.writeByte(computeDxgiFormat(vsParamDesc));
    }

    vsBlob->Release();

    ReflectionInfo psInfo(psBlob);
    psInfo->GetDesc(&desc);
    if (desc.ConstantBuffers == 1) {
        writeConstants(writer, "cbPixel", psInfo);
    } else if (desc.ConstantBuffers == 0) {
        writer.writeUInt(0);
    } else {
        throw PGE_CREATE_EX("Too many pixel constant buffers!");
    }

    u32 samplerCount = 0;
    for (UINT i = 0; i < desc.BoundResources; i++) {
        D3D11_SHADER_INPUT_BIND_DESC inputBindDesc;
        psInfo->GetResourceBindingDesc(i, &inputBindDesc);
        if (inputBindDesc.Type == D3D_SIT_SAMPLER) {
            samplerCount++;
        }
    }
    writer.writeUInt(samplerCount);

    psBlob->Release();
}

static ID3DBlob* compileDX11(const FilePath& path, const String& dxEntryPoint, const String& input) {
    ID3DBlob* compiledBlob; ID3DBlob* errorBlob;
    HRESULT hr = D3DCompile(input.cstr(), input.byteLength(), NULL, NULL, NULL, dxEntryPoint.cstr(), (dxEntryPoint.toLower() + "_5_0").cstr(), D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &compiledBlob, &errorBlob);
    if (FAILED(hr)) {
        String failure = "Compilation failed (" + String::fromInt(hr) + ")";
        if (errorBlob != NULL) {
            failure += ":\n";
            failure += (char*)errorBlob->GetBufferPointer();
            errorBlob->Release();
        }
        throw PGE_CREATE_EX(failure);
    } else {
        BinaryWriter writer(path);
        writer.writeBytes((byte*)compiledBlob->GetBufferPointer(), compiledBlob->GetBufferSize());
    }
    return compiledBlob;
}

static void compileShader(const FilePath& path) {
    String input = path.read();

    FilePath compiledPath = path.trimExtension().makeDirectory();
    compiledPath.createDirectory();

    ID3DBlob* vsBlob = compileDX11(compiledPath + "vertex.dxbc", "VS", input);
    ID3DBlob* psBlob = compileDX11(compiledPath + "pixel.dxbc", "PS", input);
    compileDX11Reflection(compiledPath + "reflection.dxri", input, vsBlob, psBlob);
}

int main(int argc, char** argv) {
    String folderName;
    if (argc < 2) {
        std::cout << "Folder containing the shaders: ";
        char chrs[128];
        std::cin >> chrs;
        folderName = chrs;
    } else {
        folderName = argv[1];
    }

    std::vector<FilePath> shaders = FilePath::fromStr(folderName).enumerateFiles();

    std::for_each(std::execution::par_unseq, shaders.begin(), shaders.end(), [](const FilePath& shader) {
        if (shader.getExtension() == "hlsl") {
            std::cout << "Compiling: " + shader.str() + '\n';
            compileShader(shader);
            std::cout << "Success: " + shader.str() + '\n';
        }
    });

    return 0;
}

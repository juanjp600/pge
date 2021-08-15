#include <d3d11.h>
#include <d3dcompiler.h>

#include <iostream>
#include <execution>
#include <functional>
#include <filesystem>

#include <PGE/Exception/Exception.h>
#include <PGE/File/BinaryWriter.h>
#include <PGE/String/Unicode.h>
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
    throw PGE_CREATE_EX("Unsupported DXGI format (" + String::fromInt(paramDesc.Mask) + ", " + String::fromInt(paramDesc.ComponentType) + ")");
}

class ReflectionInfo {
    public:
        ReflectionInfo(ID3DBlob* shader) {
            HRESULT hr = D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection);
            PGE_ASSERT(SUCCEEDED(hr), "D3DReflect failed: HRESULT " + String::fromInt(hr));
        }

        ReflectionInfo(const ReflectionInfo& other) {
            reflection = other.reflection;
            reflection->AddRef();
        }

        void operator=(const ReflectionInfo& other) {
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

struct CompileResult {
    ID3DBlob* compiledD3dBlob;
    struct HlslStruct {
        struct Member {
            String type;
            String name;

            String dxSemanticName;
            int dxSemanticIndex;
        };

        String name;
        std::vector<Member> members;
    } inputStruct;
};

static void writeConstants(BinaryWriter& writer, ReflectionInfo info) {
    D3D11_SHADER_DESC shaderDesc;
    info->GetDesc(&shaderDesc);
    writer.write<u32>(shaderDesc.ConstantBuffers);

    for (int i = 0; i < (int)shaderDesc.ConstantBuffers; i++) {
        ID3D11ShaderReflectionConstantBuffer* cBuffer = info->GetConstantBufferByIndex(i);
        D3D11_SHADER_BUFFER_DESC cBufferDesc;
        cBuffer->GetDesc(&cBufferDesc);
        writer.write<String>(cBufferDesc.Name);
        writer.write<u32>(cBufferDesc.Size);
        writer.write<u32>(cBufferDesc.Variables);
        for (int j = 0; j < (int)cBufferDesc.Variables; j++) {
            ID3D11ShaderReflectionVariable* cBufferVar = cBuffer->GetVariableByIndex(j);
            D3D11_SHADER_VARIABLE_DESC cBufferVarDesc;
            cBufferVar->GetDesc(&cBufferVarDesc);
            writer.write<String>(cBufferVarDesc.Name);
            writer.write<u32>(cBufferVarDesc.StartOffset);
            writer.write<u32>(cBufferVarDesc.Size);
        }
    }
}

namespace Parser {
    static void skip(String::Iterator& it, const std::function<bool (char16)>& predicate) {
        while (predicate(*it)) {
            it++;
        }
    }

    static void expectFixed(String::Iterator& it, char16 ch) {
        PGE_ASSERT(*it == ch, String("Expected \"") + ch + "\", found \"" + *it + '"');
        it++;
    }
}

static bool isNotDigitOrSemicolon(char16 chr) {
    return !Unicode::isDigit(chr) && (chr != L';');
}

static String extractInputStructName(const String& hlsl, const String& functionName) {
    String::Iterator iter = hlsl.begin();
    while (iter < hlsl.end()) {
        iter = hlsl.findFirst(functionName, iter) + functionName.length();
        Parser::skip(iter, Unicode::isSpace);
        if (*iter == '(') {
            iter++;
            String::Iterator start = iter;
            String::Iterator end = iter;
            Parser::skip(end, std::not_fn(Unicode::isSpace));
            return hlsl.substr(start, end);
        }
    }
    return "";
}

static CompileResult::HlslStruct parseHlslStruct(const String& hlsl, const String& structName) {
    // struct VS_INPUT {
    //     'TYPE' 'INPUT_NAME' : 'SEMANTIC_NAME''SEMANTIC_INDEX';
    //     'TYPE' 'INPUT_NAME' : 'SEMANTIC_NAME';
    //     'TYPE' 'INPUT_NAME' : 'SEMANTIC_NAME''SEMANTIC_INDEX';
    // }

    CompileResult::HlslStruct parsedStruct;
    parsedStruct.name = structName;

    String structDecl = "struct "+structName;
    String::Iterator before = hlsl.findFirst(structDecl) + structDecl.length();
    // Space before {
    Parser::skip(before, Unicode::isSpace);
    // {
    Parser::expectFixed(before, L'{');

    // Whitespace before first type
    Parser::skip(before, Unicode::isSpace);
    while (*before != L'}') {
        CompileResult::HlslStruct::Member member;

        String::Iterator after = before;
        // Type
        Parser::skip(after, std::not_fn(Unicode::isSpace));
        member.type = hlsl.substr(before, after);
        // Whitespace after type
        Parser::skip(after, Unicode::isSpace);

        before = after;
        // Member name
        Parser::skip(after, std::not_fn(Unicode::isSpace));
        member.name = hlsl.substr(before, after);

        before = after;
        // Whitespace before colon
        Parser::skip(before, Unicode::isSpace);
        Parser::expectFixed(before, L':');
        // Whitespace after colon
        Parser::skip(before, Unicode::isSpace);

        after = before;
        Parser::skip(after, isNotDigitOrSemicolon);

        member.dxSemanticName = hlsl.substr(before, after);
        if (*after == ';') {
            member.dxSemanticIndex = 0;
        } else {
            member.dxSemanticIndex = *after - '0';
            after++;
        }

        parsedStruct.members.push_back(member);

        before = after;
        // Skip semicolon
        before++;

        // Whitespace before next type
        Parser::skip(before, Unicode::isSpace);
    }

    return parsedStruct;
}

static std::vector<CompileResult::HlslStruct::Member>::const_iterator findMember(const CompileResult::HlslStruct& hlslStruct, const String& semanticName, int semanticIndex) {
    for (auto it = hlslStruct.members.begin(); it < hlslStruct.members.end(); it++) {
        if ((it->dxSemanticName == semanticName) && (it->dxSemanticIndex == semanticIndex)) { return it; }
    }
    return hlslStruct.members.end();
}

static void generateDXReflectionInformation(const FilePath& path, const CompileResult& vsCompileResult, const CompileResult& fsCompileResult) {
    BinaryWriter writer(path);

    ReflectionInfo vsInfo(vsCompileResult.compiledD3dBlob);

    D3D11_SHADER_DESC desc;
    vsInfo->GetDesc(&desc);
    writeConstants(writer, vsInfo);

    const CompileResult::HlslStruct& vsInputStruct = vsCompileResult.inputStruct;

    writer.write<u32>(desc.InputParameters);
    for (UINT i = 0; i < desc.InputParameters; ++i) {
        D3D11_SIGNATURE_PARAMETER_DESC vsParamDesc;
        vsInfo->GetInputParameterDesc(i, &vsParamDesc);

        const auto& it = findMember(vsInputStruct, vsParamDesc.SemanticName, vsParamDesc.SemanticIndex);
        PGE_ASSERT(it != vsInputStruct.members.end(), "Couldn't find semantic (" + String(vsParamDesc.SemanticName) + String::fromInt(vsParamDesc.SemanticIndex) + ")");
        writer.write<String>(it->name);
        writer.write<String>(vsParamDesc.SemanticName);
        writer.write<byte>(vsParamDesc.SemanticIndex);

        writer.write<byte>(computeDxgiFormat(vsParamDesc));
    }

    vsCompileResult.compiledD3dBlob->Release();

    ReflectionInfo psInfo(fsCompileResult.compiledD3dBlob);
    psInfo->GetDesc(&desc);
    writeConstants(writer, psInfo);

    u32 samplerCount = 0;
    for (UINT i = 0; i < desc.BoundResources; i++) {
        D3D11_SHADER_INPUT_BIND_DESC inputBindDesc;
        psInfo->GetResourceBindingDesc(i, &inputBindDesc);
        if (inputBindDesc.Type == D3D_SIT_SAMPLER) {
            samplerCount++;
        }
    }
    writer.write<u32>(samplerCount);

    fsCompileResult.compiledD3dBlob->Release();
}

static CompileResult compileDXBC(const FilePath& path, const String& dxEntryPoint, const String& hlsl) {
    CompileResult result;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(hlsl.cstr(), hlsl.byteLength(), nullptr, nullptr, nullptr, dxEntryPoint.cstr(), (dxEntryPoint.toLower() + "_5_0").cstr(),
        D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &result.compiledD3dBlob, &errorBlob);
    if (FAILED(hr)) {
        String failure = "Compilation failed (" + String::fromInt(hr) + ")";
        if (errorBlob != nullptr) {
            failure += ":\n";
            failure += (char*)errorBlob->GetBufferPointer();
            errorBlob->Release();
        }
        throw PGE_CREATE_EX(failure);
    } else {
        BinaryWriter writer(path);
        writer.writeBytes((byte*)result.compiledD3dBlob->GetBufferPointer(), (int)result.compiledD3dBlob->GetBufferSize());

        result.inputStruct = parseHlslStruct(hlsl, extractInputStructName(hlsl, dxEntryPoint));
    }
    return result;
}

static void compileShader(const FilePath& path) {
    String input = path.readText();

    FilePath compiledPath = path.trimExtension().makeDirectory();
    compiledPath.createDirectory();

    CompileResult vsResult = compileDXBC(compiledPath + "vertex.dxbc", "VS", input);
    CompileResult fsResult = compileDXBC(compiledPath + "fragment.dxbc", "PS", input); //it's called a fucking fragment shader, microsoft is wrong about this
    generateDXReflectionInformation(compiledPath + "reflection.dxri", vsResult, fsResult);
}

static void compileAndLog(const FilePath& path) {
    if (path.getExtension() == "hlsl") {
        std::cout << "Compiling: " + path.str() + '\n';
        compileShader(path);
        std::cout << "Success: " + path.str() + '\n';
    }
}

int main(int argc, char** argv) {
    String folderName;
    if (argc < 2) {
        std::cout << "Folder containing the shaders: ";
        std::cin >> folderName;
    } else {
        folderName = argv[1];
    }

    std::vector<FilePath> shaderPaths = FilePath::fromStr(folderName).enumerateFiles();

#ifdef _DEBUG
    for (auto path : shaderPaths) { compileAndLog(path); }
#else
    std::for_each(std::execution::par_unseq, shaderPaths.begin(), shaderPaths.end(), compileAndLog);
#endif

    return 0;
}

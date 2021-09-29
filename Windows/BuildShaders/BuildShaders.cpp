#include <d3d11.h>
#include <d3dcompiler.h>

#include <iostream>
#include <execution>
#include <filesystem>

#include <PGE/Exception/Exception.h>
#include <PGE/File/BinaryWriter.h>
#include <PGE/File/TextWriter.h>
#include <PGE/String/Unicode.h>

#include "CompileResult.h"
#include "Glsl.h"
#include "Vulkan.h"
#include "Parser.h"

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
    throw PGE_CREATE_EX("Unsupported DXGI format (" + String::from(paramDesc.Mask) + ", " + String::from<int>(paramDesc.ComponentType) + ")");
}

class ReflectionInfo {
    public:
        ReflectionInfo(ID3DBlob* shader) {
            HRESULT hr = D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection);
            PGE_ASSERT(SUCCEEDED(hr), "D3DReflect failed: HRESULT " + String::from(hr));
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

static void generateDXReflectionInformation(const FilePath& path, const CompileResult& vsCompileResult, const CompileResult& fsCompileResult) {
    BinaryWriter writer(path);

    ReflectionInfo vsInfo(vsCompileResult.compiledD3dBlob);

    D3D11_SHADER_DESC desc;
    vsInfo->GetDesc(&desc);
    writeConstants(writer, vsInfo);

    const CompileResult::HlslStruct& vsInputStruct = vsCompileResult.inputType;

    writer.write<u32>(desc.InputParameters);
    for (UINT i = 0; i < desc.InputParameters; ++i) {
        D3D11_SIGNATURE_PARAMETER_DESC vsParamDesc;
        vsInfo->GetInputParameterDesc(i, &vsParamDesc);

        auto it = vsInputStruct.findMember(vsParamDesc.SemanticName, vsParamDesc.SemanticIndex);
        PGE_ASSERT(it != vsInputStruct.members.end(), "Couldn't find semantic (" + String(vsParamDesc.SemanticName) + String::from(vsParamDesc.SemanticIndex) + ")");
        writer.write<String>(it->name);
        writer.write<String>(vsParamDesc.SemanticName);
        writer.write<byte>((byte)vsParamDesc.SemanticIndex);

        writer.write<byte>((byte)computeDxgiFormat(vsParamDesc));
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
        String failure = "Compilation failed (" + String::from(hr) + ")";
        if (errorBlob != nullptr) {
            failure += ":\n";
            failure += (char*)errorBlob->GetBufferPointer();
            errorBlob->Release();
        }
        throw PGE_CREATE_EX(failure);
    } else {
        BinaryWriter writer(path);
        writer.writeBytes((byte*)result.compiledD3dBlob->GetBufferPointer(), (int)result.compiledD3dBlob->GetBufferSize());

        CompileResult::extractFunctionData(hlsl, dxEntryPoint, result);
        std::vector<CompileResult::Function> funcs = CompileResult::extractFunctions(hlsl);
        for (const CompileResult::Function& f : funcs) {
            if (result.hlslFunctionBody.contains(f.name)) {
                result.functions.push_back(f);
            }
        }
        std::vector<String> cBufferNames = CompileResult::extractCBufferNames(hlsl);
        for (const String& cBufName : cBufferNames) {
            CompileResult::CBuffer cBuffer = CompileResult::parseCBuffer(hlsl, cBufName);
            if (cBuffer.usedByFunction(result.hlslFunctionBody)) {
                result.cBuffers.emplace_back(std::move(cBuffer));
            }
        }
        result.textureInputs = CompileResult::extractTextureInputs(hlsl);
        result.constants = CompileResult::extractConstants(hlsl);
    }
    return result;
}

static void compileShader(const FilePath& path) {
    String input = path.readText();

    FilePath compiledPath = path.trimExtension().makeDirectory();
    compiledPath.createDirectory();

    // TODO: The parsing inside here should only be done once.
    CompileResult vsResult = compileDXBC(compiledPath + "vertex.dxbc", "VS", input);
    CompileResult fsResult = compileDXBC(compiledPath + "fragment.dxbc", "PS", input); //it's called a fucking fragment shader, microsoft is wrong about this
    generateDXReflectionInformation(compiledPath + "reflection.dxri", vsResult, fsResult);

    Glsl::convert(compiledPath + "vertex.glsl", vsResult, Glsl::ShaderType::VERTEX);
    Glsl::convert(compiledPath + "fragment.glsl", fsResult, Glsl::ShaderType::FRAGMENT);

    Vulkan::hlslToVkHlsl(path, fsResult, vsResult);
    // TODO: FIX THIS FFS. Prevents multithreading!
    system(("glslangValidator.exe -S vert -e VS -o " + compiledPath.str() + "vert.spv -V -D " + compiledPath.str() + "hlsl.vulkan").cstr());
    system(("glslangValidator.exe -S frag -e PS -o " + compiledPath.str() + "frag.spv -V -D " + compiledPath.str() + "hlsl.vulkan").cstr());
    system(("spirv-link " + compiledPath.str() + "vert.spv " + compiledPath.str() + "frag.spv -o " + compiledPath.str() + "shader.spv").cstr());
}

static bool recompile = false;

static void compileAndLog(const FilePath& path) {
    if (path.getExtension() == "hlsl") {
        FilePath reflectFile = path.trimExtension().makeDirectory() + "reflection.dxri";
        if (!recompile && reflectFile.exists() && path.getLastModifyTime() <= reflectFile.getLastModifyTime()) {
            std::cout << "Already compiled: " + path.str() + '\n';
        } else {
            std::cout << "Compiling: " + path.str() + '\n';
            compileShader(path);
            std::cout << "Success: " + path.str() + '\n';
        }
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

#if 1
#pragma message ("RECOMPILATION IS ENABLED!!!")
    std::cout << "Recompiling..." << std::endl;
    //recompile = true;
    for (auto path : shaderPaths) { compileAndLog(path); }
#else
    std::for_each(std::execution::par_unseq, shaderPaths.begin(), shaderPaths.end(), compileAndLog);
#endif

    return 0;
}

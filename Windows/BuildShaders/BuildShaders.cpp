#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <fstream>
#include <vector>

DXGI_FORMAT computeDxgiFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc) {
    //https://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
    if (paramDesc.Mask == 1) {
        if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) { return DXGI_FORMAT_R32_UINT; }
        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) { return DXGI_FORMAT_R32_SINT; }
        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) { return DXGI_FORMAT_R32_FLOAT; }
    } else if (paramDesc.Mask <= 3) {
        if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) { return DXGI_FORMAT_R32G32_UINT; }
        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) { return DXGI_FORMAT_R32G32_SINT; }
        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) { return DXGI_FORMAT_R32G32_FLOAT; }
    } else if (paramDesc.Mask <= 7) {
        if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) { return DXGI_FORMAT_R32G32B32_UINT; }
        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) { return DXGI_FORMAT_R32G32B32_SINT; }
        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) { return DXGI_FORMAT_R32G32B32_FLOAT; }
    } else if (paramDesc.Mask <= 15) {
        if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) { return DXGI_FORMAT_R32G32B32A32_UINT; }
        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) { return DXGI_FORMAT_R32G32B32A32_SINT; }
        else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) { return DXGI_FORMAT_R32G32B32A32_FLOAT; }
    }
}


int main() {
    wchar_t input[512];
    printf("File to compile: ");
    fgetws(input,511,stdin);
    input[lstrlenW(input)-1]='\0';

    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = S_OK;

    ID3DBlob* vsBlob = nullptr;
    hr = D3DCompileFromFile(input,NULL,NULL,"VS","vs_4_0",D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&vsBlob,&errorBlob);

    if (FAILED(hr)) {
        if (errorBlob) {
            printf("%s\n", errorBlob->GetBufferPointer());
        }
        printf("FAILED TO COMPILE %ls:\n%d\n", input, hr);
        Sleep(10000);
        return hr;
    }

    std::wstring vriOutFn = input;
    vriOutFn = vriOutFn.replace(vriOutFn.find(L"shader.hlsl"), strlen("shader.hlsl"), L"vertex.dxri");

    FILE* vriOutFile = _wfopen(vriOutFn.c_str(), L"wb");

    ID3D11ShaderReflection* vsReflectionInterface = NULL;
    D3DReflect(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vsReflectionInterface);

    D3D11_SHADER_DESC vsShaderDesc;
    vsReflectionInterface->GetDesc(&vsShaderDesc);

    unsigned char inputParamCount = vsShaderDesc.InputParameters;
    fwrite(&inputParamCount, 1, 1, vriOutFile);

    for (int i = 0; i < vsShaderDesc.InputParameters; i++) {
        D3D11_SIGNATURE_PARAMETER_DESC vsSignatureParameterDesc;
        vsReflectionInterface->GetInputParameterDesc(i, &vsSignatureParameterDesc);
        int strLen = strlen(vsSignatureParameterDesc.SemanticName);
        fwrite(vsSignatureParameterDesc.SemanticName, sizeof(char), strLen + 1, vriOutFile);

        unsigned char paramIndex = vsSignatureParameterDesc.SemanticIndex;
        fwrite(&paramIndex, 1, 1, vriOutFile);

        unsigned char paramFormat = (unsigned char)computeDxgiFormat(vsSignatureParameterDesc);
        fwrite(&paramFormat, 1, 1, vriOutFile);
    }

    vsReflectionInterface->Release();

    fclose(vriOutFile);

    std::wstring vsOutFn = input;
    vsOutFn = vsOutFn.replace(vsOutFn.find(L"shader.hlsl"), strlen("shader.hlsl"), L"vertex.dxbc");

    FILE* vsOutFile = _wfopen(vsOutFn.c_str(), L"wb");

    fwrite(vsBlob->GetBufferPointer(),1,vsBlob->GetBufferSize(),vsOutFile);
    fclose(vsOutFile);
    
    ID3DBlob* psBlob = nullptr;
    hr = D3DCompileFromFile(input,NULL,NULL,"PS","ps_4_0",D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&psBlob,NULL);

    if (FAILED(hr)) {
        if (errorBlob) {
            printf("%s\n", errorBlob->GetBufferPointer());
        }
        printf("FAILED TO COMPILE %ls:\n%d\n", input, hr);
        Sleep(10000);
        return hr;
    }

    std::wstring priOutFn = input;
    priOutFn = priOutFn.replace(priOutFn.find(L"shader.hlsl"), strlen("shader.hlsl"), L"fragment.dxri");

    FILE* priOutFile = _wfopen(priOutFn.c_str(), L"wb");

    ID3D11ShaderReflection* psReflectionInterface = NULL;
    D3DReflect(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&psReflectionInterface);

    D3D11_SHADER_DESC psShaderDesc;
    psReflectionInterface->GetDesc(&psShaderDesc);

    unsigned char samplerCount = 0;
    
    for (int i=0;i<psShaderDesc.BoundResources;i++) {
        D3D11_SHADER_INPUT_BIND_DESC psInputBindDesc;
        psReflectionInterface->GetResourceBindingDesc(i, &psInputBindDesc);
        if (psInputBindDesc.Type == D3D_SIT_SAMPLER) {
            samplerCount++;
        }
    }
    fwrite(&samplerCount, 1, 1, priOutFile);

    psReflectionInterface->Release();

    fclose(priOutFile);

    std::wstring psOutFn = input;
    psOutFn = psOutFn.replace(psOutFn.find(L"shader.hlsl"),strlen("shader.hlsl"),L"fragment.dxbc");
    
    FILE* psOutFile = _wfopen(psOutFn.c_str(),L"wb");
    fwrite(psBlob->GetBufferPointer(),1,psBlob->GetBufferSize(),psOutFile);
    fclose(psOutFile);

}

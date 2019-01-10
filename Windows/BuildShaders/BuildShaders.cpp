#include <stdio.h>
#include <d3dcompiler.h>
#include <string>
#include <fstream>

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
            printf("%s\n",errorBlob->GetBufferPointer());
        }
        printf("FAILED TO COMPILE %ls:\n%d\n",input,hr);
        Sleep(10000);
        return hr;
    }

    std::wstring vsOutFn = input;
    vsOutFn = vsOutFn.replace(vsOutFn.find(L"shader.hlsl"),strlen("shader.hlsl"),L"vertex.dxbc");

    wprintf(L"%s",vsOutFn.c_str());

    FILE* vsOutFile = _wfopen(vsOutFn.c_str(),L"w");
    fwrite(vsBlob->GetBufferPointer(),1,vsBlob->GetBufferSize(),vsOutFile);
    fclose(vsOutFile);
    ID3DBlob* psBlob = nullptr;
    D3DCompileFromFile(input,NULL,NULL,"PS","ps_4_0",D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&psBlob,NULL);
    std::wstring psOutFn = input;
    psOutFn = psOutFn.replace(psOutFn.find(L"shader.hlsl"),strlen("shader.hlsl"),L"fragment.dxbc");
    FILE* psOutFile = _wfopen(psOutFn.c_str(),L"w");
    fwrite(psBlob->GetBufferPointer(),1,psBlob->GetBufferSize(),psOutFile);
    fclose(psOutFile);

}

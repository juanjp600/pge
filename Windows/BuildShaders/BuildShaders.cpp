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

HRESULT compileShader(const wchar_t* input) {
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = S_OK;

	ID3DBlob* vsBlob = nullptr;
	hr = D3DCompileFromFile(input, NULL, NULL, "VS", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vsBlob, &errorBlob);

	if (FAILED(hr)) {
		if (errorBlob) {
			printf("%s\n", errorBlob->GetBufferPointer());
		}
		printf("FAILED TO COMPILE %ls:\n%d\n", input, hr);
		return hr;
	}

	struct InputNameSemanticRelation {
		std::string memberName;
		std::string semanticName;
		int semanticIndex;
	};
	std::vector<InputNameSemanticRelation> inputNameSemanticRelations;

	std::ifstream hlslFile; hlslFile.open(input, std::ios_base::in);
	while (!hlslFile.eof()) {
		InputNameSemanticRelation insr;

		char line[256]; hlslFile.getline(line, 256);
		std::string lineStr = line;
		if (lineStr.find("struct VS_INPUT") != std::string::npos) {
			while (true) {
				hlslFile.getline(line, 256);
				lineStr = line;
				size_t colonPos = lineStr.find(":");
				if (colonPos != std::string::npos) {
					std::string memberName = "";
					std::string semanticName = "";
					bool capturingName = false;
					for (int i = colonPos - 1; i >= 0; i--) {
						if (lineStr[i] == ' ' || lineStr[i] == '\t') {
							if (capturingName) {
								break;
							}
							else {
								continue;
							}
						}
						memberName = lineStr[i] + memberName;
						capturingName = true;
					}
					for (int i = colonPos + 1; i < lineStr.size(); i++) {
						if (lineStr[i] != ' ' && lineStr[i] != '\t') {
							if (lineStr[i] >= '0' && lineStr[i] <= '9') {
								insr.memberName = memberName;
								insr.semanticName = semanticName;
								insr.semanticIndex = lineStr[i] - '0';
								inputNameSemanticRelations.push_back(insr);
								printf("%s %s%d\n", memberName.c_str(), semanticName.c_str(), insr.semanticIndex);
								break;
							}
							semanticName = semanticName + lineStr[i];
							if (i >= lineStr.size() - 1) {
								insr.memberName = memberName;
								insr.semanticName = semanticName;
								insr.semanticIndex = 0;
								inputNameSemanticRelations.push_back(insr);
								printf("%s %s%d\n", memberName.c_str(), semanticName.c_str(), insr.semanticIndex);
							}
						}
					}
				}
				size_t closingBracketPos = lineStr.find("}");
				if (closingBracketPos != std::string::npos) {
					break;
				}
			}
			break;
		}
	}
	hlslFile.close();

	std::wstring riOutFn = input;
	riOutFn = riOutFn.replace(riOutFn.find(L"shader.hlsl"), strlen("shader.hlsl"), L"reflection.dxri");

	FILE* riOutFile = _wfopen(riOutFn.c_str(), L"wb");

	ID3D11ShaderReflection* vsReflectionInterface = NULL;
	D3DReflect(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vsReflectionInterface);

	D3D11_SHADER_DESC vsShaderDesc;
	vsReflectionInterface->GetDesc(&vsShaderDesc);

	unsigned char vsCBufferCount = vsShaderDesc.ConstantBuffers;
	fwrite(&vsCBufferCount, 1, 1, riOutFile);
	for (int i = 0; i < vsShaderDesc.ConstantBuffers; i++) {
		ID3D11ShaderReflectionConstantBuffer* cBuffer = vsReflectionInterface->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC cBufferDesc;
		cBuffer->GetDesc(&cBufferDesc);
		fwrite(cBufferDesc.Name, sizeof(char), strlen(cBufferDesc.Name) + 1, riOutFile);
		unsigned char cBufferSize = cBufferDesc.Size;
		fwrite(&cBufferSize, 1, 1, riOutFile);
		unsigned char cBufferVars = cBufferDesc.Variables;
		fwrite(&cBufferVars, 1, 1, riOutFile);
		for (int j = 0; j < cBufferDesc.Variables; j++) {
			ID3D11ShaderReflectionVariable* cBufferVar = cBuffer->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC cBufferVarDesc;
			cBufferVar->GetDesc(&cBufferVarDesc);
			fwrite(cBufferVarDesc.Name, sizeof(char), strlen(cBufferVarDesc.Name) + 1, riOutFile);
			unsigned char cBufferVarOffset = cBufferVarDesc.StartOffset;
			fwrite(&cBufferVarOffset, 1, 1, riOutFile);
			unsigned char cBufferVarSize = cBufferVarDesc.Size;
			fwrite(&cBufferVarSize, 1, 1, riOutFile);
		}
	}

	unsigned char inputParamCount = vsShaderDesc.InputParameters;
	fwrite(&inputParamCount, 1, 1, riOutFile);

	for (int i = 0; i < vsShaderDesc.InputParameters; i++) {
		D3D11_SIGNATURE_PARAMETER_DESC vsSignatureParameterDesc;
		vsReflectionInterface->GetInputParameterDesc(i, &vsSignatureParameterDesc);

		std::string memberName = "unknown";
		for (int j = 0; j < inputNameSemanticRelations.size(); j++) {
			if (inputNameSemanticRelations[j].semanticName == vsSignatureParameterDesc.SemanticName &&
				inputNameSemanticRelations[j].semanticIndex == vsSignatureParameterDesc.SemanticIndex) {
				memberName = inputNameSemanticRelations[j].memberName;
				break;
			}
		}

		fwrite(memberName.c_str(), sizeof(char), memberName.size() + 1, riOutFile);

		int strLen = strlen(vsSignatureParameterDesc.SemanticName);
		fwrite(vsSignatureParameterDesc.SemanticName, sizeof(char), strLen + 1, riOutFile);

		unsigned char paramIndex = vsSignatureParameterDesc.SemanticIndex;
		fwrite(&paramIndex, 1, 1, riOutFile);

		unsigned char paramFormat = (unsigned char)computeDxgiFormat(vsSignatureParameterDesc);
		fwrite(&paramFormat, 1, 1, riOutFile);
	}

	vsReflectionInterface->Release();

	std::wstring vsOutFn = input;
	vsOutFn = vsOutFn.replace(vsOutFn.find(L"shader.hlsl"), strlen("shader.hlsl"), L"vertex.dxbc");

	FILE* vsOutFile = _wfopen(vsOutFn.c_str(), L"wb");

	fwrite(vsBlob->GetBufferPointer(), 1, vsBlob->GetBufferSize(), vsOutFile);
	fclose(vsOutFile);

	ID3DBlob* psBlob = nullptr;
	hr = D3DCompileFromFile(input, NULL, NULL, "PS", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &psBlob, NULL);

	if (FAILED(hr)) {
		if (errorBlob) {
			printf("%s\n", errorBlob->GetBufferPointer());
		}
		printf("FAILED TO COMPILE %ls:\n%d\n", input, hr);
		return hr;
	}

	ID3D11ShaderReflection* psReflectionInterface = NULL;
	D3DReflect(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&psReflectionInterface);

	D3D11_SHADER_DESC psShaderDesc;
	psReflectionInterface->GetDesc(&psShaderDesc);

	unsigned char psCBufferCount = psShaderDesc.ConstantBuffers;
	fwrite(&psCBufferCount, 1, 1, riOutFile);
	for (int i = 0; i < psShaderDesc.ConstantBuffers; i++) {
		ID3D11ShaderReflectionConstantBuffer* cBuffer = psReflectionInterface->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC cBufferDesc;
		cBuffer->GetDesc(&cBufferDesc);
		fwrite(cBufferDesc.Name, sizeof(char), strlen(cBufferDesc.Name) + 1, riOutFile);
		unsigned char cBufferSize = cBufferDesc.Size;
		fwrite(&cBufferSize, 1, 1, riOutFile);
		unsigned char cBufferVars = cBufferDesc.Variables;
		fwrite(&cBufferVars, 1, 1, riOutFile);
		for (int j = 0; j < cBufferDesc.Variables; j++) {
			ID3D11ShaderReflectionVariable* cBufferVar = cBuffer->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC cBufferVarDesc;
			cBufferVar->GetDesc(&cBufferVarDesc);
			fwrite(cBufferVarDesc.Name, sizeof(char), strlen(cBufferVarDesc.Name) + 1, riOutFile);
			unsigned char cBufferVarOffset = cBufferVarDesc.StartOffset;
			fwrite(&cBufferVarOffset, 1, 1, riOutFile);
			unsigned char cBufferVarSize = cBufferVarDesc.Size;
			fwrite(&cBufferVarSize, 1, 1, riOutFile);
		}
	}

	unsigned char samplerCount = 0;

	for (int i = 0; i < psShaderDesc.BoundResources; i++) {
		D3D11_SHADER_INPUT_BIND_DESC psInputBindDesc;
		psReflectionInterface->GetResourceBindingDesc(i, &psInputBindDesc);
		if (psInputBindDesc.Type == D3D_SIT_SAMPLER) {
			samplerCount++;
		}
	}
	fwrite(&samplerCount, 1, 1, riOutFile);

	psReflectionInterface->Release();

	fclose(riOutFile);

	std::wstring psOutFn = input;
	psOutFn = psOutFn.replace(psOutFn.find(L"shader.hlsl"), strlen("shader.hlsl"), L"fragment.dxbc");

	FILE* psOutFile = _wfopen(psOutFn.c_str(), L"wb");
	fwrite(psBlob->GetBufferPointer(), 1, psBlob->GetBufferSize(), psOutFile);
	fclose(psOutFile);

	return S_OK;
}

int main(int argc, char* argv[]) {
	wchar_t folderName[512];
	if (argc < 2) {
		printf("Folder containing the shaders: ");
		fgetws(folderName, 511, stdin);
		folderName[lstrlenW(folderName) - 1] = '\0';
	}
	else {
		for (int i = 0; i < 512; i++) {
			folderName[i] = argv[1][i];
			if (folderName[i] == '\0') {
				break;
			}
		}
	}

	WIN32_FIND_DATA findData;
	ZeroMemory(&findData, sizeof(WIN32_FIND_DATA));
	HANDLE fHandle = FindFirstFileW((std::wstring(folderName)+L"/*").c_str(), &findData);

	if (fHandle == 0) { return 0; }

	do {
		wprintf(L"%ls\n", findData.cFileName);
		DWORD attribs = GetFileAttributesW(findData.cFileName);
		if ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0 && findData.cFileName[0] != '.') {
			std::wstring fileName = (std::wstring(folderName) + L"/" + std::wstring(findData.cFileName) + L"/shader.hlsl");
			printf("Compiling %ls...\n",fileName.c_str());
			// DX11
			compileShader(fileName.c_str());
			// Vulkan
			// This is fucking ugly and should probably be done properly via Win32's ShellExecute.
			char* cmd = new char[512];
			wcstombs(cmd, (std::wstring(L"glslangValidator.exe -S vert -e VS -o ") + (std::wstring(folderName) + L"/" + std::wstring(findData.cFileName) + L"/vert.spv") + L" -V -D " + fileName).c_str(), 512);
			system(cmd);
			wcstombs(cmd, (std::wstring(L"glslangValidator.exe -S frag -e PS -o ") + (std::wstring(folderName) + L"/" + std::wstring(findData.cFileName) + L"/frag.spv") + L" -V -D " + fileName).c_str(), 512);
			system(cmd);
		}
	} while (FindNextFileW(fHandle, &findData) != 0);

	FindClose(fHandle);

	if (argc < 2) {
		system("pause");
	}

	return 0;
}

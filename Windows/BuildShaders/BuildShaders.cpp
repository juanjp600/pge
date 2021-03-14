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
	return DXGI_FORMAT_UNKNOWN;
}

HRESULT compileShader(const wchar_t* input) {
	ID3DBlob* errorBlob = nullptr;

	D3D_SHADER_MACRO shaderMacros[2] = { "[[vk::push_constant]]", "", NULL, NULL };

	HRESULT hr = S_OK;

	ID3DBlob* vsBlob = nullptr;
	hr = D3DCompileFromFile(input, shaderMacros, NULL, "VS", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vsBlob, &errorBlob);

	if (FAILED(hr)) {
		if (errorBlob) {
			printf("%s\n", (char*)errorBlob->GetBufferPointer());
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
					for (int i = colonPos + 1; i < (int)lineStr.size(); i++) {
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
							if (i >= (int)lineStr.size() - 1) {
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

	FILE* riOutFile; _wfopen_s(&riOutFile, riOutFn.c_str(), L"wb");

	ID3D11ShaderReflection* vsReflectionInterface = NULL;
	D3DReflect(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vsReflectionInterface);

	D3D11_SHADER_DESC vsShaderDesc;
	vsReflectionInterface->GetDesc(&vsShaderDesc);

	unsigned char vsCBufferCount = vsShaderDesc.ConstantBuffers;
	fwrite(&vsCBufferCount, 1, 1, riOutFile);
	for (int i = 0; i < (int)vsShaderDesc.ConstantBuffers; i++) {
		ID3D11ShaderReflectionConstantBuffer* cBuffer = vsReflectionInterface->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC cBufferDesc;
		cBuffer->GetDesc(&cBufferDesc);
		fwrite(cBufferDesc.Name, sizeof(char), strlen(cBufferDesc.Name) + 1, riOutFile);
		unsigned char cBufferSize = cBufferDesc.Size;
		fwrite(&cBufferSize, 1, 1, riOutFile);
		unsigned char cBufferVars = cBufferDesc.Variables;
		fwrite(&cBufferVars, 1, 1, riOutFile);
		for (int j = 0; j < (int)cBufferDesc.Variables; j++) {
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

	for (int i = 0; i < (int)vsShaderDesc.InputParameters; i++) {
		D3D11_SIGNATURE_PARAMETER_DESC vsSignatureParameterDesc;
		vsReflectionInterface->GetInputParameterDesc(i, &vsSignatureParameterDesc);

		std::string memberName = "unknown";
		for (int j = 0; j < (int)inputNameSemanticRelations.size(); j++) {
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

	FILE* vsOutFile; _wfopen_s(&vsOutFile, vsOutFn.c_str(), L"wb");

	fwrite(vsBlob->GetBufferPointer(), 1, vsBlob->GetBufferSize(), vsOutFile);
	fclose(vsOutFile);

	ID3DBlob* psBlob = nullptr;
	hr = D3DCompileFromFile(input, shaderMacros, NULL, "PS", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &psBlob, &errorBlob);

	if (FAILED(hr)) {
		if (errorBlob) {
			printf("%s\n", (char*)errorBlob->GetBufferPointer());
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
	for (int i = 0; i < (int)psShaderDesc.ConstantBuffers; i++) {
		ID3D11ShaderReflectionConstantBuffer* cBuffer = psReflectionInterface->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC cBufferDesc;
		cBuffer->GetDesc(&cBufferDesc);
		fwrite(cBufferDesc.Name, sizeof(char), strlen(cBufferDesc.Name) + 1, riOutFile);
		unsigned char cBufferSize = cBufferDesc.Size;
		fwrite(&cBufferSize, 1, 1, riOutFile);
		unsigned char cBufferVars = cBufferDesc.Variables;
		fwrite(&cBufferVars, 1, 1, riOutFile);
		for (int j = 0; j < (int)cBufferDesc.Variables; j++) {
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

	for (int i = 0; i < (int)psShaderDesc.BoundResources; i++) {
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

	FILE* psOutFile; _wfopen_s(&psOutFile, psOutFn.c_str(), L"wb");
	fwrite(psBlob->GetBufferPointer(), 1, psBlob->GetBufferSize(), psOutFile);
	fclose(psOutFile);

	return S_OK;
}

bool generateVulkan(const wchar_t* filename) {
	std::ifstream in;
	in.open(filename, std::ios_base::in);
	if (!in.is_open()) {
		return false;
	}

	std::vector<std::string> fragmentNames;
	std::vector<std::string> vertexNames;
	std::vector<std::string>* currVect = nullptr;
	std::vector<char> word;
	std::vector<char> word2;
	bool hadFrag = false;
	bool hadVert = false;
	while (!in.eof()) {
		char line[256]; in.getline(line, 256);
		std::string lineStr = line;
		if (currVect != nullptr) {
			if (!lineStr.compare("};")) {
				currVect = nullptr;
				continue;
			}
			int i = 0;
			while (line[i] == '\t' || line[i] == ' ') { i++; } // Before type
			if (line[i] == '/' && line[i + 1] == '/') { // It's commented out!
				continue;
			}
			while (line[i] != '\t' && line[i] != ' ') { // Type!
				word2.push_back(line[i]);
				i++;
			}
			while (line[i] == '\t' || line[i] == ' ') { i++; } // After type
			while (line[i] != ';') { // Name!
				word.push_back(line[i]);
				i++;
			}
			word.push_back('\0');
			word2.push_back('\0');
			currVect->push_back(std::string(word2.data()));
			currVect->push_back(std::string(word.data()));
			word.clear();
			word2.clear();
		} else if (hadFrag && hadVert) {
			break;
		}
		// TODO: Infer shader stage from functions the vars are used in?
		if (lineStr.find("cbuffer cbFragment") != std::string::npos) {
			if (hadFrag) {
				fragmentNames.clear();
				vertexNames.clear();
				break;
			}
			hadFrag = true;
			currVect = &fragmentNames;
		} else if (lineStr.find("cbuffer cbMatrices") != std::string::npos) {
			if (hadVert) {
				fragmentNames.clear();
				vertexNames.clear();
				break;
			}
			hadFrag = true;
			currVect = &vertexNames;
		}
	}

	if (fragmentNames.size() == 0 && vertexNames.size() == 0) {
		in.close();
		return false;
	}

	std::wstring outFile = filename;
	std::ofstream out;
	out.open(outFile.substr(0, outFile.size() - 5).append(L"_vk.hlsl").c_str(), std::ios::out);

	out << "[[vk::push_constant]]" << std::endl;
	out << "cbuffer vulkanConstants {" << std::endl;
	for (int i = 0; i < (int)vertexNames.size(); i += 2) {
		out << '\t' << vertexNames[i] << ' ' << "vert_" << vertexNames[i + 1] << ';' << std::endl;
	}
	for (int i = 0; i < (int)fragmentNames.size(); i += 2) {
		out << '\t' << fragmentNames[i] << ' ' << "frag_" << fragmentNames[i + 1] << ';' << std::endl;
	}
	out << "};";

	in.clear();
	in.seekg(0);

	// TODO: Optimize by ignoring everything outside of code blocks?
	bool inBlock = false;
	while (!in.eof()) {
		char line[256]; in.getline(line, 256);
		std::string lineStr = line;
		if (inBlock) {
			if (lineStr.find("};") != std::string::npos) {
				inBlock = false;
			}
			continue;
		}
		if (lineStr.find("cbuffer") != std::string::npos) {
			inBlock = true;
			continue;
		}
		int index = 0;
		int newIndex = 0;
		char newLine[256];
		std::vector<int> matchingVertex = std::vector<int>(vertexNames.size() / 2);
		std::vector<int> matchingFragment = std::vector<int>(fragmentNames.size() / 2);
		while (line[index] != '\0') {
			std::vector<int>* currMatching = &matchingVertex;
			std::vector<std::string>* currNames = &vertexNames;
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < (int)currMatching->size(); k++) {
					if ((*currMatching)[k] == -1) {
						(*currMatching)[k] = 0;
					} else if (line[index] == (*currNames)[k * 2 + 1][(*currMatching)[k]]) {
						(*currMatching)[k]++;
						if ((*currNames)[k * 2 + 1].size() == (*currMatching)[k] && !std::isalpha(line[index + 1])) {
							newIndex -= (*currMatching)[k] - 1;
							memcpy(&newLine[newIndex], currMatching == &matchingVertex ? "vert_" : "frag_", 5);
							memcpy(&newLine[newIndex += 5], (*currNames)[k * 2 + 1].c_str(), (*currMatching)[k]);
							newIndex += (*currMatching)[k] - 1;
						}
						// We have valid alpha chars, so we don't want to be set to -1.
						continue;
					}
					// Check for . to avoid member variables of other stuff with the same name.
					if (std::isalpha(line[index]) || line[index] == '.') {
						(*currMatching)[k] = -1;
					} else {
						(*currMatching)[k] = 0;
					}
				}
				currMatching = &matchingFragment;
				currNames = &fragmentNames;
			}
			newLine[newIndex] = line[index];
			newIndex++;
			index++;
		}
		newLine[newIndex] = '\0';
		out << std::string(newLine) << std::endl;
	}

	in.close();
	out.close();

	return true;
}

int main(int argc, char* argv[]) {
	wchar_t folderName[512];
	if (argc < 2) {
		printf("Folder containing the shaders: ");
		fgetws(folderName, 511, stdin);
		folderName[lstrlenW(folderName) - 1] = '\0';
	} else {
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
			if (generateVulkan(fileName.c_str())) {
				fileName = fileName.substr(0, fileName.size() - 5).append(L"_vk.hlsl");
			}
			char* cmd = new char[512];
			wcstombs_s(nullptr, cmd, 512, (std::wstring(L"glslangValidator.exe -S vert -e VS -o ") + (std::wstring(folderName) + L"/" + std::wstring(findData.cFileName) + L"/vert.spv") + L" -V -D " + fileName).c_str(), 512);
			system(cmd);
			wcstombs_s(nullptr, cmd, 512, (std::wstring(L"glslangValidator.exe -S frag -e PS -o ") + (std::wstring(folderName) + L"/" + std::wstring(findData.cFileName) + L"/frag.spv") + L" -V -D " + fileName).c_str(), 512);
			system(cmd);
			wcstombs_s(nullptr, cmd, 512, (std::wstring(L"spirv-link ") + (std::wstring(folderName) + L"/" + std::wstring(findData.cFileName) + L"/vert.spv ") + (std::wstring(folderName) + L"/" + std::wstring(findData.cFileName) + L"/frag.spv") + (L" -o " + (std::wstring(folderName) + L"/" + std::wstring(findData.cFileName) + L"/shader.spv"))).c_str(), 512);
			system(cmd);
		}
	} while (FindNextFileW(fHandle, &findData) != 0);

	FindClose(fHandle);

	if (argc < 2) {
		system("pause");
	}

	return 0;
}

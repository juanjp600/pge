#include "Vulkan.h"

#include <fstream>

using namespace PGE;

static const String TEXTURE_ARRAY_INTERNAL_NAME = "_PGE_INTERNAL_TEXTURES";

// TODO: Jesus fucking christ fuck all of this.
bool Vulkan::hlslToVkHlsl(const FilePath& filename, const CompileResult& fragRes, const CompileResult& vertRes) {
	std::wstring outFile = filename.str().wstr().data();
	outFile = (filename.trimExtension() + "/hlsl.vulkan").str().wstr().data();
	std::ofstream out; out.open(outFile.c_str(), std::ios::out);

	if (fragRes.textureInputs.size() > 0) {
		out << "Texture2D " + TEXTURE_ARRAY_INTERNAL_NAME + "[" << fragRes.textureInputs.size() << "];\n\n";
	}

	if (vertRes.cBuffers.size() > 0 || fragRes.cBuffers.size() > 0) {
		out << "[[vk::push_constant]]" << std::endl;
		out << "cbuffer vulkanConstants {" << std::endl;
		if (vertRes.cBuffers.size() > 0) {
			for (const CompileResult::CBuffer::Member& c : vertRes.cBuffers[0].members) {
				out << '\t' << c.type << ' ' << "vert_" << c.name << ';' << std::endl;
			}
		}
		if (fragRes.cBuffers.size() > 0) {
			for (const CompileResult::CBuffer::Member& c : fragRes.cBuffers[0].members) {
				out << '\t' << c.type << ' ' << "frag_" << c.name << ';' << std::endl;
			}
		}
		out << "}\n\n";
	}

	std::ifstream in; in.open(filename.str().wstr().data(), std::ios_base::in);

	// TODO: Optimize by ignoring everything outside of code blocks?
	bool inBlock = false;
	bool inStruct = false;
	while (!in.eof()) {
		char line[256]; in.getline(line, 256);
		std::string lineStr = line;
		if (lineStr.find("Texture2D ") != std::string::npos) {
			continue;
		}
		if (inBlock) {
			if (lineStr.find("}") != std::string::npos) {
				inBlock = false;
			}
			continue;
		}
		if (lineStr.find("cbuffer ") != std::string::npos) {
			inBlock = true;
			continue;
		}
		if (lineStr.find("struct ") != std::string::npos) {
			inStruct = true;
		}
		if (inStruct) {
			if (lineStr.find("}") != std::string::npos) {
				inStruct = false;
			}
			out << lineStr << std::endl;
			continue;
		}
		int index = 0;
		int newIndex = 0;
		char newLine[256];
		std::vector<int> matchingVertex = std::vector<int>(vertRes.cBuffers.empty() ? 0 : vertRes.cBuffers[0].members.size());
		std::vector<int> matchingFragment = std::vector<int>(fragRes.cBuffers.empty() ? 0 : fragRes.cBuffers[0].members.size());
		while (line[index] != '\0') {
			std::vector<int>* currMatching = &matchingVertex;

			const std::vector<CompileResult::CBuffer>* currBuffs = &vertRes.cBuffers;
			for (int j = 0; j < 2; j++) {
				if (!currBuffs->empty()) {
					const std::vector<CompileResult::CBuffer::Member>* currNames = &currBuffs[0][0].members;
						for (int k = 0; k < (int)currMatching->size(); k++) {
							if ((*currMatching)[k] == -1) {
								(*currMatching)[k] = 0;
							} else if ((*currMatching)[k] < (*currNames)[k].name.length() && line[index] == *(*currNames)[k].name.charAt((*currMatching)[k])) {
								(*currMatching)[k]++;
								if ((*currNames)[k].name.length() == (*currMatching)[k] && !std::isalpha(line[index + 1])) {
									newIndex -= (*currMatching)[k] - 1;
									memcpy(&newLine[newIndex], currMatching == &matchingVertex ? "vert_" : "frag_", 5);
									memcpy(&newLine[newIndex += 5], (*currNames)[k].name.cstr(), (*currMatching)[k]);
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
				}
				currMatching = &matchingFragment;
				currBuffs = &fragRes.cBuffers;
			}
			newLine[newIndex] = line[index];
			newIndex++;
			index++;
		}
		newLine[newIndex] = '\0';
		String pgeNewLine = (char*)newLine;
		for (int i = 0; i < fragRes.textureInputs.size(); i++) {
			pgeNewLine = pgeNewLine.replace(fragRes.textureInputs[i] + ".Sample", TEXTURE_ARRAY_INTERNAL_NAME + "[" + String::from(i) + "].Sample");
		}
		out << pgeNewLine << std::endl;
	}

	in.close();
	out.close();

	return true;
}

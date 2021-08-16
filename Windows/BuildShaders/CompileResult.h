#ifndef BUILDSHADERS_COMPILERESULT_H_INCLUDED
#define BUILDSHADERS_COMPILERESULT_H_INCLUDED

#include <d3dcompiler.h>
#include <PGE/String/String.h>

#include <vector>

struct CompileResult {
    ID3DBlob* compiledD3dBlob;
    struct HlslStruct {
        struct Member {
            PGE::String type;
            PGE::String name;

            PGE::String dxSemanticName;
            int dxSemanticIndex;
        };

        PGE::String name;
        std::vector<Member> members;

        std::vector<CompileResult::HlslStruct::Member>::const_iterator findMember(const PGE::String& semanticName, int semanticIndex) const;
    };
    struct CBuffer {
        struct Member {
            PGE::String type;
            PGE::String name;
        };

        PGE::String name;
        std::vector<Member> members;

        bool usedByFunction(const PGE::String& functionBody);
    };
    HlslStruct inputType; PGE::String inputParameterName;
    HlslStruct returnType;
    std::vector<CBuffer> cBuffers;
    PGE::String hlslFunctionBody;


    static CompileResult::HlslStruct parseHlslStruct(const PGE::String& hlsl, const PGE::String& structName);

    static std::vector<PGE::String> extractCBufferNames(const PGE::String& hlsl);
    static CompileResult::CBuffer parseCBuffer(const PGE::String& hlsl, const PGE::String& cBufferName);

    static void extractFunctionData(const PGE::String& hlsl, const PGE::String& functionName, CompileResult& compileResult);
};

#endif

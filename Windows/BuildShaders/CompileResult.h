#ifndef BUILDSHADERS_COMPILERESULT_H_INCLUDED
#define BUILDSHADERS_COMPILERESULT_H_INCLUDED

#ifdef USE_D3D
#include <d3dcompiler.h>
#endif
#include <PGE/String/String.h>

#include <vector>

struct CompileResult {
    #ifdef USE_D3D
    ID3DBlob* compiledD3dBlob;
    #endif
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
    struct Constant {
        PGE::String type;
        PGE::String name;
        PGE::String value;
    };
    struct Function {
        PGE::String name;
        PGE::String func;
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
    std::vector<Function> functions;
    std::vector<Constant> constants;
    std::vector<CBuffer> cBuffers;
    std::vector<PGE::String> textureInputs;
    PGE::String hlslFunctionBody;


    static CompileResult::HlslStruct parseHlslStruct(const PGE::String& hlsl, const PGE::String& structName);

    static const std::vector<Function> extractFunctions(const PGE::String& hlsl);
    static const std::vector<Constant> extractConstants(const PGE::String& hlsl);
    static const std::vector<PGE::String> extractHlslDeclNames(const PGE::String& hlsl, const PGE::String& declType);
    static const std::vector<PGE::String> extractCBufferNames(const PGE::String& hlsl);
    static const std::vector<PGE::String> extractTextureInputs(const PGE::String& hlsl);
    static CompileResult::CBuffer parseCBuffer(const PGE::String& hlsl, const PGE::String& cBufferName);

    static void extractFunctionData(const PGE::String& hlsl, const PGE::String& functionName, CompileResult& compileResult);
};

#endif // BUILDSHADERS_COMPILERESULT_H_INCLUDED

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
    };
    struct CBuffer {
        struct Member {
            PGE::String type;
            PGE::String name;
        };

        PGE::String name;
        std::vector<Member> members;
    };
    HlslStruct inputType; PGE::String inputParameterName;
    HlslStruct returnType;
    std::vector<CBuffer> cBuffers;
    PGE::String hlslFunctionBody;
};

#endif

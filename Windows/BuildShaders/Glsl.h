#ifndef BUILDSHADERS_GLSL_H_INCLUDED
#define BUILDSHADERS_GLSL_H_INCLUDED

#include <PGE/String/String.h>
#include <PGE/File/FilePath.h>
#include <PGE/File/TextWriter.h>

#include "CompileResult.h"

namespace Glsl {
    enum class ShaderType {
        VERTEX,
        FRAGMENT
    };

    PGE::String hlslToGlslTypes(const PGE::String& hlsl);
    void writeHlslFuncs(PGE::TextWriter& writer, const PGE::String& funcBody);
    PGE::String prefixIfRequired(const PGE::String& varName, const PGE::String& varKind, ShaderType shaderType);
    void writeStructDef(PGE::TextWriter& writer, const CompileResult::HlslStruct& hlslStruct);
    void writeStructReturnAux(PGE::TextWriter& writer, const CompileResult::HlslStruct& hlslStruct, ShaderType shaderType);
    void writeStructAsVars(PGE::TextWriter& writer, const CompileResult::HlslStruct& hlslStruct, const PGE::String& varKind, ShaderType shaderType);
    void writeMain(PGE::TextWriter& writer, const CompileResult& compileResult, ShaderType shaderType);
    void convert(const PGE::FilePath& path, const CompileResult& compileResult, ShaderType shaderType);
}

#endif

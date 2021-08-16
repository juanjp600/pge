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

    void convert(const PGE::FilePath& path, const CompileResult& compileResult, ShaderType shaderType);
}

#endif

#ifndef BS_VULKAN_H_INCLUDED
#define BS_VULKAN_H_INCLUDED

#include <PGE/File/FilePath.h>

#include "CompileResult.h"

namespace Vulkan {
	bool hlslToVkHlsl(const PGE::FilePath& filename, const CompileResult& fragRes, const CompileResult& vertRes);
}

#endif // BS_VULKAN_H_INCLUDED

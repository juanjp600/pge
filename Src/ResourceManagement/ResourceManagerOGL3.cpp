#include "ResourceManagerOGL3.h"

#include "../Graphics/GraphicsOGL3.h"

using namespace PGE;

ResourceManagerOGL3::ResourceManagerOGL3(GraphicsOGL3& gfx) : graphics(gfx) { }

ResourceManagerOGL3::~ResourceManagerOGL3() {
    graphics.takeGlContext();
}

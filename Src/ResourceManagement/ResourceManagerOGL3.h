#ifndef PGEINTERNAL_RESOURCEMANAGER_OGL3
#define PGEINTERNAL_RESOURCEMANAGER_OGL3

#include <PGE/ResourceManagement/ResourceManager.h>

#include "../Graphics/GraphicsInternal.h"

namespace PGE {

class Graphics;
class GraphicsOGL3;

class ResourceManagerOGL3 : public ResourceManager, private GraphicsReferencer<GraphicsOGL3> {
    public:
        ResourceManagerOGL3(Graphics* gfx);
        ~ResourceManagerOGL3();
};

}

#endif // PGEINTERNAL_RESOURCEMANAGER_OGL3

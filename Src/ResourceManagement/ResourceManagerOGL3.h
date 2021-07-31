#ifndef PGEINTERNAL_RESOURCEMANAGER_OGL3
#define PGEINTERNAL_RESOURCEMANAGER_OGL3

#include <PGE/ResourceManagement/ResourceManager.h>

#include "../Graphics/GraphicsInternal.h"

namespace PGE {

class GraphicsOGL3;

class ResourceManagerOGL3 : public ResourceManager {
    public:
        ResourceManagerOGL3(Graphics* gfx);
        ~ResourceManagerOGL3();
    private:
        GraphicsOGL3* graphics;
};

}

#endif // PGEINTERNAL_RESOURCEMANAGER_OGL3

#ifndef PGEINTERNAL_RESOURCEMANAGER_OGL3
#define PGEINTERNAL_RESOURCEMANAGER_OGL3

#include "../Graphics/GraphicsInternal.h"
#include <PGE/ResourceManagement/ResourceManager.h>

namespace PGE {

class GraphicsOGL3;
class ResourceManagerOGL3 : public ResourceManager {
    private:
        GraphicsOGL3& graphics;
    public:
        ResourceManagerOGL3(GraphicsOGL3& gfx);
        ~ResourceManagerOGL3();
};

}

#endif // PGEINTERNAL_RESOURCEMANAGER_OGL3

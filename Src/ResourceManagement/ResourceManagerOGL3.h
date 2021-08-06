#ifndef PGEINTERNAL_RESOURCEMANAGER_OGL3
#define PGEINTERNAL_RESOURCEMANAGER_OGL3

#include "../Graphics/GraphicsInternal.h"
#include <PGE/ResourceManagement/ResourceManager.h>

namespace PGE {

class ResourceManagerOGL3 : public ResourceManager, private GraphicsReferencer<class GraphicsOGL3> {
    public:
        ResourceManagerOGL3(class Graphics& gfx);
        ~ResourceManagerOGL3();
};

}

#endif // PGEINTERNAL_RESOURCEMANAGER_OGL3

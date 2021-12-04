#ifndef PGEINTERNAL_RESOURCEMANAGER_OGL3_H_INCLUDED
#define PGEINTERNAL_RESOURCEMANAGER_OGL3_H_INCLUDED

#include <PGE/ResourceManagement/ResourceManager.h>

namespace PGE {

class ResourceManagerOGL3 : public ResourceManager {
    private:
        class GraphicsOGL3& graphics;

    public:
        ResourceManagerOGL3(class Graphics& gfx);
        ~ResourceManagerOGL3();
};

}

#endif // PGEINTERNAL_RESOURCEMANAGER_OGL3_H_INCLUDED

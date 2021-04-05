#ifndef PGEINTERNAL_RESOURCEMANAGER_VK
#define PGEINTERNAL_RESOURCEMANAGER_VK

#include <ResourceManagement/ResourceManager.h>

namespace PGE {

    class Graphics;
    class GraphicsVK;

    // TODO: Refactor to give access to device from here?
    class ResourceManagerVK : public ResourceManager {
        private:
        GraphicsVK* graphics;

        public:
        ResourceManagerVK(Graphics* gfx, int size);
        ~ResourceManagerVK();
    };

}

#endif // PGEINTERNAL_RESOURCEMANAGER_VK

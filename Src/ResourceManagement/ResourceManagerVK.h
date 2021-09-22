#ifndef PGEINTERNAL_RESOURCEMANAGER_VK
#define PGEINTERNAL_RESOURCEMANAGER_VK

#include <PGE/ResourceManagement/ResourceManager.h>

namespace PGE {

// TODO: Refactor to give access to device from here?
class ResourceManagerVK : public ResourceManager {
    private:
        class GraphicsVK& graphics;

    public:
        ResourceManagerVK(class Graphics& gfx);
        ~ResourceManagerVK();
};

}

#endif // PGEINTERNAL_RESOURCEMANAGER_VK

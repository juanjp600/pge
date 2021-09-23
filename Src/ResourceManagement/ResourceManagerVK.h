#ifndef PGEINTERNAL_RESOURCEMANAGER_VK
#define PGEINTERNAL_RESOURCEMANAGER_VK

#include <PGE/ResourceManagement/ResourceManager.h>

namespace PGE {

// TODO: Refactor to give access to device from here?
class ResourceManagerVK : public ResourceManager {
    private:
        class GraphicsVK& graphics;

        void trashInternal(const std::list<ResourceBase*>::iterator& it);

    public:
        ResourceManagerVK(class Graphics& gfx);
        ~ResourceManagerVK();

        // TODO: Remove with C++20.
        template <typename T>
        void trash(const ResourceView<T>& view) {
            if (!view.isHoldingResource()) { return; }
            trashInternal(getIterator(view));
        }
};

}

#endif // PGEINTERNAL_RESOURCEMANAGER_VK

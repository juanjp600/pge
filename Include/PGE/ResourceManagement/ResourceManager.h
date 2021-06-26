#ifndef PGE_RESOURCEMANAGER_H_INCLUDED
#define PGE_RESOURCEMANAGER_H_INCLUDED

#include <vector>

#include "Resource.h"

namespace PGE {

class ResourceBase;
class ResourceManager {
    private:
        std::vector<ResourceBase*> resources;

        size_t size;

    public:
        ResourceManager(size_t sz);
        virtual ~ResourceManager();

        template <class T>
        ResourceView<decltype(T::resource)> takeOwnership(T* res) {
            static_assert(std::is_base_of<ResourceBase, T>::value);
            resources.push_back(res);
            PGE_ASSERT(size >= resources.size(), "Tried to add resource to full ResourceManager");
            return ResourceView<decltype(T::resource)>(*res);
        }

        template <class T>
        void deleteResource(T internalResource) {
            // Static assertion is likely not possible here due to ResourceViews's template.
            for (auto it = resources.end(); it > resources.begin();) {
                it--;
                Resource<T>* specifiedResource = static_cast<Resource<T>*>(*it);
                if (specifiedResource != nullptr && specifiedResource->get() == internalResource) {
                    delete specifiedResource;
                    resources.erase(it);
                    return;
                }
            }
        }

        template <class T>
        void deleteResourcefromReference(ResourceView<T> view) {
            if (!view.isHoldingResource()) {
                return;
            }

            deleteResource(view.get());
        }

        void increaseSize(size_t count);
};

}

#endif // PGE_RESOURCEMANAGER_H_INCLUDED

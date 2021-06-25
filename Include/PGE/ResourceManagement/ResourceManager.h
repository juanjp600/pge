#ifndef PGE_RESOURCEMANAGER_H_INCLUDED
#define PGE_RESOURCEMANAGER_H_INCLUDED

#include <vector>

#include "Resource.h"

namespace PGE {

class ResourceBase;
class ResourceManager {
    private:
        std::vector<ResourceBase*> resources;

#ifdef DEBUG
        // We need this, because the C++ specification states that reserve on a vector might increase the capacity beyond the specified size.
        size_t size;
#endif

    public:
        ResourceManager(size_t sz);
        virtual ~ResourceManager();

        template <class T, class... Args>
        ResourceView<decltype(T::resource)> addNewResource(Args... args) {
#ifdef DEBUG
            PGE_ASSERT(size > resources.size(), "Tried to add resource to full ResourceManager");
#endif
            T* res = new T(args...);
            resources.push_back(res);
            return ResourceView<decltype(T::resource)>(*res);
        }

        template <class T>
        void deleteResource(T internalResource) {
            // Static assertion is likely not possible here due to ResourceViews's template.
            for (auto it = resources.end(); it > resources.begin();) {
                it--;
                // TODO: We likely want to replace the dynamic cast here.
                // This method itself isn't really used anyways, so giving views a ptr to their resource will probably be good enough.
                Resource<T>* specifiedResource = dynamic_cast<Resource<T>*>(*it);
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

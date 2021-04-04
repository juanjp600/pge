#ifndef PGE_RESOURCEMANAGER_H_INCLUDED
#define PGE_RESOURCEMANAGER_H_INCLUDED

#include <vector>

#include <Exception/Exception.h>

#include "Resource.h"
#include "ResourceReference.h"

namespace PGE {

class ResourceBase;
class ResourceManager {
    private:
        std::vector<ResourceBase*> resources;

#ifdef DEBUG
        // We need this, because the C++ specification states that reserve on a vector might increase the capacity beyond the specified size.
        int size;
#endif

    public:
        ResourceManager(int sz);
        virtual ~ResourceManager();

        void addResource(ResourceBase* resource);

        template <class T>
        void deleteResource(ResourceReference<T>& reference) {
            if (!reference.isHoldingResource()) {
                return;
            }

            T internalResource = reference;
            for (auto it = resources.end(); it > resources.begin();) {
                it--;
                Resource<T>* specifiedResource = dynamic_cast<Resource<T>*>(*it);
                if (specifiedResource != nullptr && specifiedResource->getResource() == internalResource) {
                    delete specifiedResource;
                    resources.erase(it);
                    reference.invalidate();
                    return;
                }
            }
        }

        void increaseSize(int count);
};

}

#endif // PGE_RESOURCEMANAGER_H_INCLUDED

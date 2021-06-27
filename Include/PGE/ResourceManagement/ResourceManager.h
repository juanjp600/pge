#ifndef PGE_RESOURCEMANAGER_H_INCLUDED
#define PGE_RESOURCEMANAGER_H_INCLUDED

#include <vector>

#include "Resource.h"

namespace PGE {

class ResourceBase;
class ResourceManager {
    private:
        std::list<ResourceBase*> resources;

    public:
        virtual ~ResourceManager();

        template <class T, class... Args>
        ResourceView<decltype(T::resource)> addNewResource(Args... args) {
            static_assert(std::is_base_of<ResourceBase, T>::value);
            T* res = new T(args...);
            resources.push_back(res);
            return ResourceView<decltype(T::resource)>(*res, --resources.end());
        }

        template <class T>
        void deleteResource(ResourceView<T> view) {
            if (!view.isHoldingResource()) {
                return;
            }

            delete *view.getPosition();
            resources.erase(view.getPosition());
        }
};

}

#endif // PGE_RESOURCEMANAGER_H_INCLUDED

#ifndef PGE_RESOURCEMANAGER_H_INCLUDED
#define PGE_RESOURCEMANAGER_H_INCLUDED

#include <vector>

#include "NoHeap.h"
#include "Resource.h"

namespace PGE {

class ResourceBase;
class ResourceManager : private NoHeap {
    private:
        std::list<ResourceBase*> resources;

    public:
        virtual ~ResourceManager();

        template <typename T, typename... Args>
        typename T::View addNewResource(Args&&... args) {
            static_assert(std::is_base_of<ResourceBase, T>::value);
            T* res = new T(std::forward<Args>(args)...);
            resources.push_back(res);
            std::list<ResourceBase*>::iterator lastResourceIter = resources.end();
            lastResourceIter--;
            return ResourceView(res->get(), lastResourceIter);
        }

        template <typename T>
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

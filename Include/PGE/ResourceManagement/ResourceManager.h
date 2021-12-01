#ifndef PGE_RESOURCEMANAGER_H_INCLUDED
#define PGE_RESOURCEMANAGER_H_INCLUDED

#include <list>

#include "Resource.h"
#include "RawWrapper.h"

namespace PGE {

class ResourceManager {
    protected:
        std::list<ResourceBase*> resources;

        template <std::semiregular T>
        const std::list<ResourceBase*>::iterator& getIterator(const ResourceView<T>& v) const {
            return v.iterator;
        }

        template <std::derived_from<ResourceBase> T, typename... Args>
        typename T::View add(Args&&... args) {
            T* res = new T(std::forward<Args>(args)...);
            resources.emplace_back(res);
            return ResourceView(res->get(), --resources.end());
        }

    public:
        ~ResourceManager() {
            for (auto it = resources.rbegin(); it != resources.rend(); it++) {
                delete* it;
            }
        }

        template <std::derived_from<ResourceBase> T, typename... Args>
        typename T::View addNewResource(Args&&... args) {
            return add<T>(std::forward<Args>(args)...);
        }

        template <typename T>
        typename RawWrapper<T>::View addPointer(T* ptr) {
            return add<RawWrapper<T>>(ptr);
        }

        template <typename T, typename... Args>
        typename RawWrapper<T>::View addNew(Args&&... args) {
            return add<RawWrapper<T>>(std::forward<Args>(args)...);
        }

        template <std::semiregular T>
        void deleteResource(ResourceView<T> view) {
            if (!view.isHoldingResource()) {
                return;
            }

            delete *view.iterator;
            resources.erase(view.iterator);
        }
};

}

#endif // PGE_RESOURCEMANAGER_H_INCLUDED

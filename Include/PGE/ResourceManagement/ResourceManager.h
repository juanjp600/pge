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

        template <class T,
                  typename... Args>
        decltype(auto) addNewResource(Args... args) {
            //TODO: This is cancer and AIDS.
            //decltype(T::resource) doesn't work because
            //the resource field is protected. We are
            //not going to make it public just for this.
            //We should probably turn this into
            //takeOwnership(Resource<T>* res) instead
            //of dealing with bullshit templates not
            //doing what we want.
            static_assert(std::is_base_of<ResourceBase, T>::value);
            T* res = new T(args...);
            resources.push_back(res);
            std::list<ResourceBase*>::iterator lastResourceIter = resources.end();
            lastResourceIter--;
            return ResourceView(res->get(), lastResourceIter);
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

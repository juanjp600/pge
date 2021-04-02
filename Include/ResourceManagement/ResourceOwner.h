#ifndef PGE_RESOURCEOWNER_H_INCLUDED
#define PGE_RESOURCEOWNER_H_INCLUDED

#include "Resource.h"

namespace PGE {

template <class T>
class ResourceOwner {
    private:
        Resource<T>* resource = nullptr;
        void clean() {
            if (resource != nullptr) { delete resource; }
        }
    public:
        ~ResourceOwner() { clean(); }
        ResourceOwner<T>& operator=(const ResourceOwner<T>& other) = delete;
        operator const T&() const { return resource->getResource(); }
        const T& operator->() const { return resource->getResource(); }
        const T* operator&() const { return resource->getResourcePointer(); }
        bool isHoldingResource() const { return resource != nullptr; }
        void set(Resource<T>* newResource) {
            clean();
            resource = newResource;
        }
};

}

#endif // PGE_RESOURCEOWNER_H_INCLUDED

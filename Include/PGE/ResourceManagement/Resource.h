#ifndef PGE_RESOURCE_H_INCLUDED
#define PGE_RESOURCE_H_INCLUDED

#include <type_traits>

#include "ResourceView.h"

namespace PGE {

class ResourceBase {
    public:
        virtual ~ResourceBase() = default;
};

// Only inherit publicly and use the designated macro.
template <class T>
class Resource : public ResourceBase {
    protected:
        T resource;

    public:
        typedef ResourceView<T> View;

        Resource() = default;
        Resource(const Resource<T>& other) = delete;
        Resource<T>& operator=(const Resource<T>& other) = delete;

        operator const T& () const { return resource; }
        // Force cast.
        const T& get() const { return resource; }
        
        template <class = typename std::enable_if<std::is_pointer<T>::value>::type>
        const T& operator->() const {
            return resource;
        }

        template <class = typename std::enable_if<!std::is_pointer<T>::value>::type>
        const T* operator->() const {
            return &resource;
        }

        const T* operator&() const { return &resource; }
};

}

#endif // PGE_RESOURCE_H_INCLUDED

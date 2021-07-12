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
template <typename T>
class Resource : public ResourceBase {
    protected:
        T resource;

    public:
        using View = ResourceView<T>;

        Resource() = default;
        Resource(const Resource<T>& other) = delete;
        Resource<T>& operator=(const Resource<T>& other) = delete;

        // Force cast.
        const T& get() const { return resource; }
        
        operator const T& () const { return get(); }

        template <typename Y = T, typename = typename std::enable_if<std::is_pointer<Y>::value>::type>
        const T& operator->() const { return get(); }

        template <typename Y = T, typename = typename std::enable_if<std::negation<std::is_pointer<Y>>::value>::type>
        const T* operator->() const { return &get(); }

        const T* operator&() const { return &get(); }
};

}

#endif // PGE_RESOURCE_H_INCLUDED

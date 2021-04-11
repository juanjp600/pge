#ifndef PGE_RESOURCE_H_INCLUDED
#define PGE_RESOURCE_H_INCLUDED

#include <type_traits>

#include "ResourceReference.h"

#define __RES_MNGMT__REF_FACT_METH(Res) \
typedef ResourceReference<decltype(resource)> Ref; \
\
template <class... Args> \
static Ref createRef(ResourceManager& resMngr, Args... args) { \
    static_assert(std::is_base_of<ResourceBase, Res>::value); \
    Res* res = new Res(args...); \
    resMngr.addResource(res); \
    return Ref(*res); \
}

namespace PGE {

class ResourceBase {
    public:
        virtual ~ResourceBase() { }
};

// Only inherit publicly and use the designated macro.
template <class T>
class Resource : public ResourceBase {
    protected:
        T resource;
    public:
        Resource() { }
        Resource(const Resource<T>& other) = delete;
        Resource<T>& operator=(const Resource<T>& other) = delete;

        operator const T& () const { return resource; }
        // Force cast.
        const T& operator()() const { return resource; }
        const T& operator->() const { return resource; }
        const T* operator&() const { return &resource; }
};

}

#endif // PGE_RESOURCE_H_INCLUDED

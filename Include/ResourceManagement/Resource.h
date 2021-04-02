#ifndef PGE_RESOURCE_H_INCLUDED
#define PGE_RESOURCE_H_INCLUDED

#include <type_traits>

#include "ResourceReference.h"

namespace PGE {

template <typename T>
std::true_type isRef(ResourceReference<T>);
std::false_type isRef(...);

#define __RES_MNGMT__REF_FACT_METH(Res, Ref) \
template <class... Args> \
static Ref createRef(ResourceManager& resMngr, Args... args) { \
    static_assert(std::is_base_of<ResourceBase, Res>::value); \
    static_assert(decltype(isRef(std::declval<Ref>()))::value); \
    Res* res = new Res(args...); \
    resMngr.addResource(res); \
    return Ref(res->getResource()); \
}

class ResourceBase {
    public:
        virtual ~ResourceBase() { }
};

// Derivatives of this should only ever be accessed via a ResourceOwner or ResourceReference with a ResourceManager!!!
template <class T>
class Resource : public ResourceBase {
    protected:
        T resource;
    public:
        Resource<T>& operator=(const Resource<T>& other) = delete;
        const T& getResource() const { return resource; }
        const T* getResourcePointer() const { return &resource; }
};

}

#endif // PGE_RESOURCE_H_INCLUDED

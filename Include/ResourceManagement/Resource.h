#ifndef PGE_RESOURCE_H_INCLUDED
#define PGE_RESOURCE_H_INCLUDED

#include <type_traits>

#define __RES_MNGMT__REF_FACTORY_METHOD(Res, Ref) \
template <class... Args> \
static Ref createRef(ResourceManager& resMngr, Args... args) { \
    static_assert(std::is_base_of<ResourceReferenceBase, Ref>::value); \
    Res* res = new Res(args...); \
    resMngr.addResource(res); \
    return Ref(*res); \
}

namespace PGE {

class ResourceBase {
    public:
        ResourceBase() { }
        virtual ~ResourceBase() { }
};

template <class T>
class Resource : public ResourceBase {
    protected:
        T resource;
    public:
        operator const T& () const { return resource; }
        const T& operator->() const { return resource; }
        Resource<T>& operator=(const Resource<T>& other) = delete;
};

}

#endif // PGE_RESOURCEOWNER_H_INCLUDED

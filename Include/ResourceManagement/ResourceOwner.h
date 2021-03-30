#ifndef PGE_RESOURCEOWNER_H_INCLUDED
#define PGE_RESOURCEOWNER_H_INCLUDED

#include <type_traits>

#define __RES_MNGMT__REF_METH__NO_DFT_CTOR(Owner, Ref) \
template <class... Args> \
static Ref createRef(ResourceManager& resMngr, Args... args) { \
    static_assert(std::is_default_constructible<Owner>::value); \
    static_assert(std::is_base_of<ResourceReferenceBase, Ref>::value); \
    Owner* owner = new Owner(args...); \
    resMngr.addResource(owner); \
    return Ref(*owner); \
}

#define __RES_MNGMT__REF_METH(Owner, Ref) \
__RES_MNGMT__REF_METH__NO_DFT_CTOR(Owner, Ref) \
\
Owner() { \
    holdsResource = false; \
}

namespace PGE {

class ResourceOwnerBase {
    protected:
        bool holdsResource;
    public:
        // True by DEFAULT!
        // Macro should be used.
        ResourceOwnerBase() { holdsResource = true; }
        virtual ~ResourceOwnerBase() { }

        bool isHoldingResource() const { return holdsResource; }
};

template <class T>
class ResourceOwner : public ResourceOwnerBase {
    protected:
        T resource;
    public:
        operator const T& () const { return resource; }
        const T& operator->() const { return resource; }
};

}

#endif // PGE_RESOURCEOWNER_H_INCLUDED

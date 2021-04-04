#ifndef PGE_RESOURCEREFERENCE_H_INCLUDED
#define PGE_RESOURCEREFERENCE_H_INCLUDED

#include <cassert>

namespace PGE {

template <class T>
class ResourceReference {
    private:
        T internalResource;
        bool holdsResource = false;
    public:
        ResourceReference() { }
        ResourceReference(T res) { internalResource = res; holdsResource = true; }
        operator const T&() const { assert(holdsResource); return internalResource; }
        // Force cast.
        const T& operator()() const { assert(holdsResource); return internalResource; }
        const T& operator->() const { assert(holdsResource); return internalResource; }
        const T* operator&() const { assert(holdsResource); return &internalResource; }
        bool isHoldingResource() const { return holdsResource; }
        void invalidate() { holdsResource = false; }
};

}

#endif // PGE_RESOURCEREFERENCE_H_INCLUDED

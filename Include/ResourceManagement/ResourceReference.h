#ifndef PGE_RESOURCEREFERENCE_H_INCLUDED
#define PGE_RESOURCEREFERENCE_H_INCLUDED

#include <type_traits>

#include <Exception/Exception.h>

namespace PGE {

template <class T>
class ResourceReference {
    private:
        T internalResource;
        bool holdsResource = false;
    public:
        ResourceReference() = default;
        ResourceReference(T res) { internalResource = res; holdsResource = true; }

        operator const T& () const { PGE_ASSERT(holdsResource, "Reference not filled"); return internalResource; }
        // Force cast.
        const T& operator()() const { PGE_ASSERT(holdsResource, "Reference not filled"); return internalResource; }
        
        template <class = typename std::enable_if<std::is_pointer<T>::value>::type>
        const T& operator->() const {
            PGE_ASSERT(holdsResource, "Reference not filled");
            return internalResource;
        }

        template <class = typename std::enable_if<!std::is_pointer<T>::value>::type>
        const T* operator->() const {
            PGE_ASSERT(holdsResource, "Reference not filled");
            return &internalResource;
        }
        
        const T* operator&() const { PGE_ASSERT(holdsResource, "Reference not filled"); return &internalResource; }

        bool isHoldingResource() const { return holdsResource; }
};

}

#endif // PGE_RESOURCEREFERENCE_H_INCLUDED

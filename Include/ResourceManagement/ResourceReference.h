#ifndef PGE_RESOURCEREFERENCE_H_INCLUDED
#define PGE_RESOURCEREFERENCE_H_INCLUDED

#include <type_traits>

#include <Exception/Exception.h>

namespace PGE {

template <class T>
class ResourceView {
    private:
        T internalResource;
        bool holdsResource = false;
    public:
        ResourceView() = default;
        ResourceView(T res) { internalResource = res; holdsResource = true; }

        operator const T& () const { return get(); }
        // Force cast.
        const T& get() const { PGE_ASSERT(holdsResource, "Reference not filled"); return internalResource; }
        
        template <class = typename std::enable_if<std::is_pointer<T>::value>::type>
        const T& operator->() const { return get(); }

        template <class = typename std::enable_if<!std::is_pointer<T>::value>::type>
        const T* operator->() const { return &get(); }
        
        const T* operator&() const { return &get(); }

        bool isHoldingResource() const { return holdsResource; }
};

}

#endif // PGE_RESOURCEREFERENCE_H_INCLUDED

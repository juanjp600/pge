#ifndef PGE_RESOURCEVIEW_H_INCLUDED
#define PGE_RESOURCEVIEW_H_INCLUDED

#include <list>
#include <type_traits>

#include <PGE/Exception/Exception.h>

namespace PGE {

class ResourceBase;
template <typename T>
class ResourceView {
    friend class ResourceManager;

    private:
        T internalResource;
        std::list<ResourceBase*>::iterator iterator;
        bool holdsResource = false;

    public:
        ResourceView() = default;
        ResourceView(T res, const std::list<ResourceBase*>::iterator& iter) { internalResource = res; holdsResource = true; iterator = iter; }

        // Force cast.
        const T& get() const { PGE_ASSERT(holdsResource, "Reference not filled"); return internalResource; }

        operator const T& () const { return get(); }

        template <typename Y = T, typename = typename std::enable_if<std::is_pointer<Y>::value>::type>
        const T& operator->() const { return get(); }

        template <typename Y = T, typename = typename std::enable_if<std::negation<std::is_pointer<Y>>::value>::type>
        const T* operator->() const { return &get(); }

        const T* operator&() const { return &get(); }

        bool isHoldingResource() const { return holdsResource; }
};

}

#endif // PGE_RESOURCEVIEW_H_INCLUDED

#ifndef PGE_RESOURCEVIEW_H_INCLUDED
#define PGE_RESOURCEVIEW_H_INCLUDED

#include <list>
#include <type_traits>

#include <PGE/Exception/Exception.h>

namespace PGE {

class ResourceBase;
template <class T>
class ResourceView {
    private:
        T internalResource;
        std::list<ResourceBase*>::iterator position;
        bool holdsResource = false;

    public:
        ResourceView() = default;
        ResourceView(T res, std::list<ResourceBase*>::iterator pos) { internalResource = res; holdsResource = true; position = pos; }

        // Force cast.
        const T& get() const { PGE_ASSERT(holdsResource, "Reference not filled"); return internalResource; }

        operator const T& () const { return get(); }

        template <class Y = T, typename = typename std::enable_if<std::is_pointer<Y>::value>::type>
        const T& operator->() const { return get(); }

        template <class Y = T, typename = typename std::enable_if<std::negation<std::is_pointer<Y>>::value>::type>
        const T* operator->() const { return &get(); }

        const T* operator&() const { return &get(); }

        bool isHoldingResource() const { return holdsResource; }

        std::list<ResourceBase*>::iterator getPosition() { return position; }
};

}

#endif // PGE_RESOURCEVIEW_H_INCLUDED

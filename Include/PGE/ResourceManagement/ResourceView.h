#ifndef PGE_RESOURCEVIEW_H_INCLUDED
#define PGE_RESOURCEVIEW_H_INCLUDED

#include <list>
#include <type_traits>

#include <PGE/Exception/Exception.h>
#include <PGE/Types/TemplateEnableIf.h>

namespace PGE {

class ResourceBase;
template <std::semiregular T>
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
        const T& get() const { asrt(holdsResource, "Reference not filled"); return internalResource; }

        operator const T&() const { return get(); }

        PGE_TEMPLATE_ENABLE_IF(std::is_pointer<Y>)
        const T& operator->() const { return get(); }

        PGE_TEMPLATE_ENABLE_IF(std::negation<std::is_pointer<Y>>)
        const T* operator->() const { return &get(); }

        const T* operator&() const { return &get(); }

        bool isHoldingResource() const { return holdsResource; }
};

}

#endif // PGE_RESOURCEVIEW_H_INCLUDED

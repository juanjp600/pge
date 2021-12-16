#ifndef PGE_RESOURCE_H_INCLUDED
#define PGE_RESOURCE_H_INCLUDED

#include <type_traits>
#include <concepts>

#include "ResourceView.h"
#include <PGE/Types/PolymorphicHeap.h>
#include <PGE/Types/TemplateEnableIf.h>

namespace PGE {

class ResourceBase : private PolymorphicHeap { };

template <std::semiregular T>
class Resource : public ResourceBase {
    protected:
        T resource;

    public:
        using View = ResourceView<T>;

        Resource() = default;

        // Force cast.
        const T& get() const { return resource; }
        
        operator const T&() const { return get(); }

        PGE_TEMPLATE_ENABLE_IF(std::is_pointer<Y>::value)
        const T& operator->() const { return get(); }

        PGE_TEMPLATE_ENABLE_IF(std::negation<std::is_pointer<Y>>::value)
        const T* operator->() const { return &get(); }

        const T* operator&() const { return &get(); }
};

}

#endif // PGE_RESOURCE_H_INCLUDED

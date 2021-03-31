#ifndef PGE_DX11RESOURCE_H_INCLUDED
#define PGE_DX11RESOURCE_H_INCLUDED

#include <ResourceManagement/Resource.h>

namespace PGE {

template <class T>
class DX11Resource : public Resource<T> {
    protected:
        virtual ~DX11Resource() override {
            static_assert(std::is_pointer<T>::value);
            static_assert(std::is_convertible<T, IUnknown*>::value);
            ((IUnknown*)this->resource)->Release();
        }
};

}

#endif // PGE_DX11RESOURCE_H_INCLUDED

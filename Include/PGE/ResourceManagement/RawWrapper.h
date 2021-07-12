#ifndef PGE_RAWWRAPPER_H_INCLUDED
#define PGE_RAWWRAPPER_H_INCLUDED

#include <PGE/ResourceManagement/Resource.h>

namespace PGE {

template <typename T>
class RawWrapper : public Resource<T*> {
    public:
        RawWrapper(T* ptr) {
            this->resource = ptr;
        }

        template <typename... Args>
        RawWrapper(Args... args) {
            this->resource = new T(args...);
        }

        ~RawWrapper() {
            delete this->resource;
        }
};

}

#endif // PGE_RAWWRAPPER_H_INCLUDED

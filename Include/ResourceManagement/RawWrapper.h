#ifndef PGE_RAW_WRAPPER_H_INCLUDED
#define PGE_RAW_WRAPPER_H_INCLUDED

#include <ResourceManagement/Resource.h>

namespace PGE {

template <class T>
class RawWrapper : public Resource<T*> {
    public:
        RawWrapper(T* ptr) {
            this->resource = ptr;
        }

        template <class... Args>
        RawWrapper(Args... args) {
            this->resource = new T(args...);
        }

        ~RawWrapper() {
            delete this->resource;
        }
};

}

#endif // PGE_RAW_WRAPPER_H_INCLUDED

#ifndef PGE_RAWWRAPPER_H_INCLUDED
#define PGE_RAWWRAPPER_H_INCLUDED

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

#endif // PGE_RAWWRAPPER_H_INCLUDED

#ifndef PGE_RESOURCEOWNER_H_INCLUDED
#define PGE_RESOURCEOWNER_H_INCLUDED

namespace PGE {

template <class T>
class ResourceOwner {
    private:
        T* resource = nullptr;
    public:
        ~ResourceOwner() { if (resource != nullptr) { delete resource; } }
        operator const T* () const { return resource; }
        const T& operator->() const { return *resource; }
        bool isHoldingResource() const { return resource != nullptr; }
        ResourceOwner<T>& operator=(const ResourceOwner<T>& other) = delete;
        void operator=(T* newResource) {
            if (resource != nullptr) { delete resource; }
            resource = newResource;
        }
};

}

#endif
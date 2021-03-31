#ifndef PGE_RESOURCEOWNER_H_INCLUDED
#define PGE_RESOURCEOWNER_H_INCLUDED

namespace PGE {

template <class T>
class ResourceOwner {
    private:
        T* resource = nullptr;
        void clean() {
            if (resource != nullptr) { delete resource; }
        }
    public:
        ~ResourceOwner() { clean(); }
        // TODO: See if we can replace the pointer here, kinda stinky.
        operator const T*() const { return resource; }
        const T& operator->() const { return *resource; }
        bool isHoldingResource() const { return resource != nullptr; }
        ResourceOwner<T>& operator=(const ResourceOwner<T>& other) = delete;
        void set(T* newResource) {
            clean();
            resource = newResource;
        }
};

}

#endif // PGE_RESOURCEOWNER_H_INCLUDED

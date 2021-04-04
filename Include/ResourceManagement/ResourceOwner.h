#ifndef PGE_RESOURCEOWNER_H_INCLUDED
#define PGE_RESOURCEOWNER_H_INCLUDED

namespace PGE {

template <typename Internal, typename ResourceChild>
class ResourceOwner {
    private:
        ResourceChild* resource = nullptr;
        void clean() {
            if (resource != nullptr) { delete resource; }
        }
    public:
        ResourceOwner() { }
        ResourceOwner(const ResourceOwner<Internal, ResourceChild>& other) = delete;
        ResourceOwner<Internal, ResourceChild>& operator=(const ResourceOwner<Internal, ResourceChild>& other) = delete;
        ~ResourceOwner() { clean(); }
        operator const Internal&() const { return resource->getResource(); }
        // Force cast.
        const Internal& operator()() const { return resource->getResource(); }
        const Internal& operator->() const { return resource->getResource(); }
        const Internal* operator&() const { return resource->getResourcePointer(); }
        bool isHoldingResource() const { return resource != nullptr; }
        template <typename... Args>
        void fillNew(Args... args) {
            clean();
            resource = new ResourceChild(args...);
        }
};

}

#endif // PGE_RESOURCEOWNER_H_INCLUDED

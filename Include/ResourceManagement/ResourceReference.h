#ifndef PGE_RESOURCEREFERENCE_H_INCLUDED
#define PGE_RESOURCEREFERENCE_H_INCLUDED

namespace PGE {

template <class T>
class ResourceReference {
    private:
        T internalResource;
        bool holdsResource = false;
    public:
        ResourceReference() { }
        ResourceReference(T res) { internalResource = res; holdsResource = true; }
        operator const T&() const { return internalResource; }
        // Force cast.
        const T& operator()() const { return internalResource; }
        const T& operator->() const { return internalResource; }
        const T* operator&() const { return &internalResource; }
        bool isHoldingResource() const { return holdsResource; }
};

}

#endif // PGE_RESOURCEREFERENCE_H_INCLUDED

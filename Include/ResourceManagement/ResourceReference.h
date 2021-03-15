#ifndef PGE_RESOURCEREFERENCE_H_INCLUDED
#define PGE_RESOURCEREFERENCE_H_INCLUDED

namespace PGE {

class ResourceReferenceBase {};

template <class T>
class ResourceReference : public ResourceReferenceBase {
    private:
        T internalResource;
        bool holdsResource = false;
    public:
        ResourceReference() { }
        ResourceReference(T res) { internalResource = res; holdsResource = true; }
        T& operator->() { return internalResource; }
        T& get() { return internalResource; }
        const T& operator->() const { return internalResource; }
        const T& get() const { return internalResource; }
        bool isHoldingResource() const { return holdsResource; }
};

template <class T>
class ResourceRefVector {
    private:
        std::vector<T> elements;

        ResourceRefVector(int sz) {
            elements.resize(sz);
        }
    public:
        class Element {
            private:
                T* elementPtr;
            public:
                Element(T& element) {
                    elementPtr = &element;
                }

                void operator=(const ResourceReference<T>& other) {
                    *elementPtr = other.get();
                }

                operator ResourceReference<T>() {
                    return ResourceReference<T>(*elementPtr);
                }

                T& get() { return *elementPtr; }
                T& operator->() const { return *elementPtr; }
        };

        ResourceRefVector() {}

        static ResourceRefVector<T> withSize(int sz) {
            return ResourceRefVector<T>(sz);
        }

        Element operator[](int i) {
            return Element(elements[i]);
        }

        void add(ResourceReference<T> element) {
            elements.push_back(element.get());
        }

        T* data() {
            return elements.data();
        }

        int size() {
            return (int)elements.size();
        }

        void clear() {
            elements.clear();
        }
};

}

#endif
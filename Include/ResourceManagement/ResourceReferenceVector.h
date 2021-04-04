#ifndef PGE_RESOURCEREFERENCEVECTOR_H_INCLUDED
#define PGE_RESOURCEREFERENCEVECTOR_H_INCLUDED

namespace PGE {

template <class T>
class ResourceReferenceVector {
    private:
        std::vector<T> elements;
        ResourceReferenceVector(int sz) {
            elements.resize(sz);
        }

    public:
        ResourceReferenceVector() { }
        static ResourceReferenceVector<T> withSize(int sz) {
            return ResourceReferenceVector<T>(sz);
        }

        T& operator[](int i) {
            return elements[i];
        }

        void add(ResourceReference<T> element) {
            elements.push_back(element);
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

#endif // PGE_RESOURCEREFERENCEVECTOR_H_INCLUDED

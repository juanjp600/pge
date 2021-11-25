#ifndef PGE_POLYMORPHICHEAP_H_DEFINED
#define PGE_POLYMORPHICHEAP_H_DEFINED

namespace PGE {

class PolymorphicHeap {
    public:
        PolymorphicHeap(const PolymorphicHeap&) = delete;
        PolymorphicHeap& operator=(const PolymorphicHeap&) = delete;

    protected:
        PolymorphicHeap() = default;
        virtual ~PolymorphicHeap() = default;
};

}

#endif // PGE_POLYMORPHICHEAP_H_DEFINED

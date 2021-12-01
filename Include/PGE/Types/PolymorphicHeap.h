#ifndef PGE_POLYMORPHICHEAP_H_INCLUDED
#define PGE_POLYMORPHICHEAP_H_INCLUDED

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

#endif // PGE_POLYMORPHICHEAP_H_INCLUDED

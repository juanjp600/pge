#ifndef PGE_NOHEAP_H_DEFINED
#define PGE_NOHEAP_H_DEFINED

namespace PGE {

/// A mixin class intended to prevent heap allocation of a class.
class NoHeap {
    public:
#ifndef PGE_REQUIRE_HEAP
        void* operator new(size_t) = delete;
        void* operator new[](size_t) = delete;
        void operator delete(void*) = delete;
        void operator delete[](void*) = delete;
#endif
    protected:
        NoHeap() = default;
};

}

#endif // PGE_NOHEAP_H_DEFINED

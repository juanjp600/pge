#ifndef PGE_RANDOM_H_INCLUDED
#define PGE_RANDOM_H_INCLUDED

#include <PGE/Types/Types.h>

namespace PGE {

// xoshiro128**
// https://prng.di.unimi.it/
// CC0
class Random {
    public:
        Random();
        Random(u64 seed);

        u32 next();
        bool nextBool();
        float nextFloat();
        u32 nextInt(u32 max);
        u32 nextInt(u32 min, u32 max);

    private:
        u32 state[4];

        void* operator new(size_t) = delete;
        void* operator new[](size_t) = delete;
        void operator delete(void*) = delete;
        void operator delete[](void*) = delete;
};

}

#endif // PGE_RANDOM_H_INCLUDED

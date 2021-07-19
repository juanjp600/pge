#ifndef PGE_RANDOM_H_INCLUDED
#define PGE_RANDOM_H_INCLUDED

#include <PGE/ResourceManagement/NoHeap.h>
#include <PGE/Types/Types.h>

namespace PGE {

// xoshiro128**
// https://prng.di.unimi.it/
// CC0
class Random : private NoHeap {
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
};

}

#endif // PGE_RANDOM_H_INCLUDED

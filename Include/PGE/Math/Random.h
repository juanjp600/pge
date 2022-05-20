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
        float nextGaussian();
        float nextGaussian(float mean, float standardDeviation);
        float nextGaussian(float mean, float standardDeviation, float min, float max);
        float nextGaussianInRange(float min, float max);
        u32 nextInt(u32 max);
        i32 nextInt(i32 min, i32 max);

    private:
        u32 state[4];
};

}

#endif // PGE_RANDOM_H_INCLUDED

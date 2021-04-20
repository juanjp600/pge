#ifndef PGE_RANDOM_H_INCLUDED
#define PGE_RANDOM_H_INCLUDED

#include <cstdint>

namespace PGE {

// xoshiro128**
// https://prng.di.unimi.it/
// CC0
class Random {
    public:
        Random();
        Random(uint64_t seed);

        uint32_t next();
        bool nextBool();
        float nextFloat();
        uint32_t nextInt(uint32_t max);
        uint32_t nextInt(uint32_t min, uint32_t max);

    private:
        uint32_t state[4];
};

}

#endif // PGE_RANDOM_H_INCLUDED

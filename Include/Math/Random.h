#ifndef PGE_RANDOM_H_INCLUDED
#define PGE_RANDOM_H_INCLUDED

#include <cstdint>

namespace PGE {

// Middle Square Weyl Sequence PRNG
// https://arxiv.org/abs/1704.00358
class Random {
    public:
        Random();
        Random(uint64_t seed);

        uint32_t next();
        bool nextBool();
        float nextFloat();
        double nextDouble();
        uint32_t nextInt(uint32_t max);
        uint32_t nextInt(uint32_t min, uint32_t max);

    private:
        Random(uint64_t seed, uint64_t start);
        
        uint64_t state;
        uint64_t weyl;
        uint64_t seed;
};

}

#endif // PGE_RANDOM_H_INCLUDED

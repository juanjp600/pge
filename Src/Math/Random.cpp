#include <Math/Random.h>

#include <chrono>
#include <iostream>

static const float size32Bit = UINT32_MAX + 1.f;
static const double size64Bit = UINT64_MAX + 1.0;

namespace PGE {

    Random::Random(uint64_t seed, uint64_t start) {
        state = start;
        weyl = start;
        this->seed = seed;
    }

    Random::Random() : Random(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

    Random::Random(uint64_t seed) {
        std::chrono::time_point lol = std::chrono::steady_clock::now();

        Random r = Random(0x56e9d7a3d6234c87, seed);

        // Throw away a few values to ensure randomness.
        for (int i = 0; i < 5; i++) {
            r.next();
        }

        uint64_t newSeed = r.next() | ((uint64_t) r.next() << 32) | 1;

        uint64_t replacement = r.next();

        for (int i = 0; i < 64; i += 4) {
            uint64_t j = 0xFui64 << i;
            if ((newSeed & j) == 0) {
                do {
                    newSeed |= ((replacement & 0xF) << i);
                    replacement >>= 4;
                    if (replacement == 0) {
                        replacement = r.next();
                    }
                } while ((newSeed & j) == 0);
            }
        }

        std::cout << (std::chrono::steady_clock::now() - lol).count() << std::endl;

        std::printf("Seed: %llx\n", newSeed);

        state = newSeed;
        weyl = newSeed;
        this->seed = newSeed | 1;
    }

    uint32_t Random::next() {
        state *= state;
        state += (weyl += seed);
        return state = (state >> 32) | (state << 32);
    }

    // true | false
    bool Random::nextBool() {
        return (next() & 1) == 1;
    }

    // [0, 1)
    float Random::nextFloat() {
        return (float)next() / size32Bit;
    }

    // [0, 1)
    double Random::nextDouble() {
        return (double)((next() << 21) ^ next()) / size64Bit;
    }

    // [0, max]
    uint32_t Random::nextInt(uint32_t max) {
        return nextFloat() * max;
    }

    // [min, max)
    uint32_t Random::nextInt(uint32_t min, uint32_t max) {
        return nextFloat() * (max - min) + min;
    }

}

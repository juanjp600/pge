#include <Math/Random.h>

#include <chrono>

#include <Exception/Exception.h>

static uint32_t rotl(uint32_t x, int8_t k) {
    return (x << k) | (x >> (32 - k));
}

namespace PGE {

    Random::Random() : Random(std::chrono::high_resolution_clock::now().time_since_epoch().count()) { }

    // SplitMix64
    // CC0
    Random::Random(uint64_t sd) {
        uint64_t* high = (uint64_t*)state;
        for (int i = 0; i < 2; i++) {
            sd += 0x9e3779b97f4a7c15;
            uint64_t z = sd;
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            high[i] = z ^ (z >> 31);
        }
    }

    uint32_t Random::next() {
        uint32_t temp = state[1] << 9;

        state[2] ^= state[0];
        state[3] ^= state[1];
        state[1] ^= state[2];
        state[0] ^= state[3];

        state[2] ^= temp;

        state[3] = rotl(state[3], 11);

        return rotl(state[1] * 5, 7) * 9;
    }

    // true | false
    bool Random::nextBool() {
        return (next() & 1) == 1;
    }

    // [0, 1)
    float Random::nextFloat() {
        // 23 bits of significand + implicit bit.
        // 32 - 24 = 8
        return (next() >> 8) * 0x1.0p-24f;
    }

    // [0, max]
    uint32_t Random::nextInt(uint32_t max) {
        /**
        * 
        * Debiased Integer Multiplication after Lemire
        * with faster threshold-based discarding and optimized modulo.
        * 
        * See https://www.pcg-random.org/posts/bounded-rands.html
        * Slightly modified for use in pge.
        * 
        * The MIT License (MIT)
        *
        * Copyright (c) 2018 Melissa E. O'Neill
        *
        * Permission is hereby granted, free of charge, to any person obtaining a
        * copy of this software and associated documentation files (the "Software"),
        * to deal in the Software without restriction, including without limitation
        * the rights to use, copy, modify, merge, publish, distribute, sublicense,
        * and/or sell copies of the Software, and to permit persons to whom the
        * Software is furnished to do so, subject to the following conditions:
        *
        * The above copyright notice and this permission notice shall be included in
        * all copies or substantial portions of the Software.
        *
        * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
        * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
        * DEALINGS IN THE SOFTWARE.
        * 
        */
        uint64_t product = (uint64_t)next() * (uint64_t)max;
        uint32_t lowerProduct = (uint32_t)product;
        if (lowerProduct < max) {
            // (2^32 - max) % max
            uint32_t maxMod = 0 - max;
            if (maxMod >= max) {
                maxMod -= max;
                if (maxMod >= max) {
                    maxMod %= max;
                }
            }
            while (lowerProduct < maxMod) {
                product = (uint64_t)next() * (uint64_t)max;
                lowerProduct = (uint32_t)product;
            }
        }
        // Return higher bits of the product.
        return product >> 32;
    }

    // [min, max)
    uint32_t Random::nextInt(uint32_t min, uint32_t max) {
        PGE_ASSERT(min <= max, "min > max (min: " + String::fromInt(min) + ", max: " + String::fromInt(max) + ")");
        return nextInt(max - min) + min;
    }

}

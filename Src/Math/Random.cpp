#include <PGE/Math/Random.h>

#include <chrono>

#include <PGE/Exception/Exception.h>

using namespace PGE;

static u32 rotl(u32 x, i8 k) {
    return (x << k) | (x >> (32 - k));
}

namespace PGE {

    Random::Random() : Random(std::chrono::high_resolution_clock::now().time_since_epoch().count()) { }

    // SplitMix64
    // CC0
    Random::Random(u64 sd) {
        u64* high = (u64*)state;
        for (int i : Range(2)) {
            sd += 0x9e3779b97f4a7c15;
            u64 z = sd;
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            high[i] = z ^ (z >> 31);
        }
    }

    u32 Random::next() {
        u32 temp = state[1] << 9;

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

    // Gaussian distribution with mean 0 and standard deviation 1
    // https://www.alanzucconi.com/2015/09/16/how-to-sample-from-a-gaussian-distribution/
    float Random::nextGaussian() {
        float v1; float v2; float s;
        do {
            v1 = 2 * nextFloat() - 1; // (-1, 1)
            v2 = 2 * nextFloat() - 1;
            s = v1 * v1 + v2 * v2;
        } while (s >= 1 || s == 0);
        return v1 * std::sqrt(-2 * std::log(s) / s);
    }

    float Random::nextGaussian(float mean, float standardDeviation) {
        return mean + nextGaussian() * standardDeviation;
    }

    // Gaussian-like distribution in range [min, max]
    float Random::nextGaussianInRange(float min, float max) {
        constexpr float STD_DEV_FROM_MEAN = 3.5f;
        float res;
        do {
            res = nextGaussian(min + (max - min) / 2, (max - min) / 2 / STD_DEV_FROM_MEAN);
        } while (res < min || res > max);
        return res;
    }

    float Random::nextGaussian(float mean, float standardDeviation, float min, float max) {
        float res;
        do {
            res = nextGaussian(mean, standardDeviation);
        } while (res < min || res > max);
        return res;
    }

    // [0, max)
    u32 Random::nextInt(u32 max) {
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
        u64 product = (u64)next() * (u64)max;
        u32 lowerProduct = (u32)product;
        if (lowerProduct < max) {
            // (2^32 - max) % max
            u32 maxMod = 0 - max;
            if (maxMod >= max) {
                maxMod -= max;
                if (maxMod >= max) {
                    maxMod %= max;
                }
            }
            while (lowerProduct < maxMod) {
                product = (u64)next() * (u64)max;
                lowerProduct = (u32)product;
            }
        }
        // Return higher bits of the product.
        return product >> 32;
    }

    // [min, max)
    i32 Random::nextInt(i32 min, i32 max) {
        PGE_ASSERT(min <= max, "min > max (min: " + String::from(min) + ", max: " + String::from(max) + ")");
        return nextInt(max - min) + min;
    }

}

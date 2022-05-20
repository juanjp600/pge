#ifndef PGE_MATH_H_INCLUDED
#define PGE_MATH_H_INCLUDED

#include <cmath>
#include <concepts>
#include <numeric>

#include <PGE/Exception/Exception.h>

namespace PGE {

/// Various mathematical utility that is either not present,
/// or unsatisfactory in the standard library.
namespace Math {
	constexpr float PI = 3.1415926535f;
	constexpr float E  = 2.7182818284f;
    /// The default difference two floats are allowed to have in order to be considered "equal".
    /// @see #PGE::Math::equalFloats
    constexpr float EPSILON_DEFAULT = 0.001f;

    /// Converts degrees to radians.
    constexpr float degToRad(float degree) noexcept {
        return degree * Math::PI / 180.0f;
    }

    /// Converts radians to degrees.
    constexpr float radToDeg(float radians) noexcept {
        return radians * 180.0f / Math::PI;
    }

    /// Two values are considered equal if |val - other| < epsilon.
    constexpr bool equalFloats(float val, float other, float epsilon = EPSILON_DEFAULT) noexcept {
        float diff = val - other;
        if (diff >= 0) {
            return diff < epsilon;
        } else {
            return -diff < epsilon;
        }
    }

    template <std::unsigned_integral T>
    constexpr T roundUp(T value, T nearest) noexcept {
        if (value % nearest == 0) {
            return value;
        }
        return (value / nearest + 1) * nearest;
    }

    template <std::integral T>
    constexpr T ceilLog2(T value) {
        bool flip;
        if constexpr (std::numeric_limits<T>::is_signed) {
            flip = value < 0;
            if (flip) {
                value = -value;
            }
        }
        PGE_ASSERT(value != 0, "value mustn't be 0");
        T ret = 0;
        for (T i = 1; i < value; i *= 2) { ret++; }
        if constexpr (std::numeric_limits<T>::is_signed) {
            return ret * (flip ? -1 : 1);
        } else {
            return ret;
        }
    }

    template <std::floating_point F>
    constexpr bool isNaN(F value) {
        return value != value;
    }

    template <typename T>
    constexpr T abs(T value) {
        return value > 0 ? value : -value;
    }

    template <std::floating_point F>
    constexpr F sin(F value) {
        if (std::is_constant_evaluated()) {
            PGE_ASSERT(!isNaN(value), "value is NaN");
            PGE_ASSERT(abs(value) != std::numeric_limits<F>::infinity(), "value is infinity");
            if (value < 0) { value = -value + PI; }
            while (value >= 2 * PI) {
                value -= 2 * PI;
            }
            // Taylor series
            F oldV;
            F newV = value;
            uint64_t i = 0;
            F running = value;
            do {
                oldV = newV;
                i += 2;
                running *= value * value / (i * (i + 1));
                if (i % 4 != 0) { newV -= running; }
                else { newV += running; }
            } while (oldV != newV);
            return newV;
        } else {
            return std::sin(value);
        }
    }

    template <std::floating_point F>
    constexpr F cos(F value) {
        if (std::is_constant_evaluated()) {
            PGE_ASSERT(!isNaN(value), "value is NaN");
            PGE_ASSERT(abs(value) != std::numeric_limits<F>::infinity(), "value is infinity");
            if (value < 0) { value = -value; }
            while (value >= 2 * PI) {
                value -= 2 * PI;
            }
            // Taylor series
            F oldV;
            F newV = 1;
            uint64_t i = 0;
            F running = 1;
            do {
                oldV = newV;
                i += 2;
                running *= value * value / (i * (i - 1));
                if (i % 4 != 0) { newV -= running; }
                else { newV += running; }
            } while (oldV != newV);
            return newV;
        } else {
            return std::cos(value);
        }
    }

    template <std::floating_point F>
    constexpr F sqrt(F value) {
        if (std::is_constant_evaluated()) {
            PGE_ASSERT(!isNaN(value), "value is NaN");
            PGE_ASSERT(abs(value) != std::numeric_limits<F>::infinity(), "value is infinity");
            PGE_ASSERT(value > 0, "value must be > 0");
            // Babylonian method
            F oldV;
            F newV = value;
            do {
                oldV = newV;
                newV = (newV + value / newV) / 2;
            } while (oldV != newV);
            return newV;
        } else {
            return std::sqrt(value);
        }
    }
}

}

#endif // PGE_MATH_H_INCLUDED

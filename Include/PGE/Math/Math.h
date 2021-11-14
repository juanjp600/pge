#ifndef PGE_MATH_H_INCLUDED
#define PGE_MATH_H_INCLUDED

namespace PGE {

/// Various mathematical utility that is either not present,
/// or unsatisfactory in the standard library.
namespace Math {
	constexpr float PI = 3.1415926535f;
	constexpr float E =  2.7182818284f;
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

    template <typename T>
    constexpr T roundUp(T value, T nearest) noexcept {
        static_assert(!std::numeric_limits<T>::is_signed);
        if (value % nearest == 0) {
            return value;
        }
        return (value / nearest + 1) * nearest;
    }

    template <typename T>
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

    /// Round down a float.
    /// Rounding down is defined as always rounding towards 0.
    /// 
    /// The standard implementation returns a float, which we consider cringe.
    constexpr int floor(float val) noexcept {
        int i = (int)val;
        if (i > val) { i--; }
        return i;
    }

    /// Round up a float.
    /// Rounding up is defined as always rounding towards +/-∞.
    /// 
    /// The standard implementation returns a float, which we consider cringe.
    constexpr int ceil(float val) noexcept {
        int i = (int)val;
        if (i < val) { i++; }
        return i;
    }
}

}

#endif // PGE_MATH_H_INCLUDED

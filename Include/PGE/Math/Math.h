#ifndef PGE_MATH_H_INCLUDED
#define PGE_MATH_H_INCLUDED

namespace PGE {

/// Various mathematical utility that is either not present,
/// or unsatisfactory in the standard library.
namespace Math {
	constexpr float PI = 3.1415926535f;
	constexpr float E =  2.7182818284f;
    constexpr float EPSILON_DEFAULT = 0.001f;

    constexpr float degToRad(float degree) {
        return degree * Math::PI / 180.0f;
    }

    constexpr float radToDeg(float radians) {
        return radians * 180.0f / Math::PI;
    }

    /// Two values are considered equal if |val - other| < epsilon.
    constexpr bool equalFloats(float val, float other, float epsilon = EPSILON_DEFAULT) {
        float diff = val - other;
        if (diff >= 0) {
            return diff < epsilon;
        }
        else {
            return -diff < epsilon;
        }
    }

    constexpr int floor(float val) {
        int i = (int)val;
        if (i > val) { i--; }
        return i;
    }

    constexpr int ceil(float val) {
        int i = (int)val;
        if (i < val) { i++; }
        return i;
    }
}

}

#endif // PGE_MATH_H_INCLUDED

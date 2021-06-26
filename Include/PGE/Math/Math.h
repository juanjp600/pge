#ifndef PGE_MATH_H_INCLUDED
#define PGE_MATH_H_INCLUDED

#include <PGE/Exception/Exception.h>

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
    float degToRad(float degrees);
    /// Converts radians to degrees.
    float radToDeg(float radians);

    /// Two values are considered equal if |val - other| < epsilon.
    bool equalFloats(float val, float other, float epsilon = EPSILON_DEFAULT);

    /// Round down a float.
    /// Rounding down is defined as always rounding towards 0.
    float floor(float val);
    float ceil(float val);
}

}

#endif // PGE_MATH_H_INCLUDED

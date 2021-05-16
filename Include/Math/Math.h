#ifndef PGE_MATH_H_INCLUDED
#define PGE_MATH_H_INCLUDED

#include <Exception/Exception.h>

namespace PGE {

namespace Math {
	constexpr float PI = 3.1415926535f;
	constexpr float E =  2.7182818284f;

    float degToRad(float degrees);
    float radToDeg(float radians);

    bool equalFloats(float val, float other);

    int floor(float val);
    int ceil(float val);
}

}

#endif // PGE_MATH_H_INCLUDED

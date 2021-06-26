#include <PGE/Math/Math.h>

using namespace PGE;

float Math::degToRad(float degree) {
    return degree * Math::PI / 180.0f;
}

float Math::radToDeg(float radians) {
    return radians * 180.0f / Math::PI;
}

bool Math::equalFloats(float val, float other, float epsilon) {
    return abs(val - other) < epsilon;
}

float Math::floor(float val) {
    int i = (int)val;
    if (i > val) { i--; }
    return i;
}

float Math::ceil(float val) {
    int i = (int)val;
    if (i < val) { i++; }
    return i;
}

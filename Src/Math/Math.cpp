#include <Math/Math.h>

using namespace PGE;

constexpr float MARGIN_ERROR = 0.001f;

float Math::degToRad(float degree) {
    return degree * Math::PI / 180.0f;
}

float Math::radToDeg(float radians) {
    return radians * 180.0f / Math::PI;
}

bool Math::equalFloats(float val, float other) {
    return abs(val - other) < MARGIN_ERROR;
}

int Math::floor(float val) {
    int i = (int)val;
    if (i > val) { i--; }
    return i;
}

int Math::ceil(float val) {
    int i = (int)val;
    if (i < val) { i++; }
    return i;
}

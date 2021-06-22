#include <PGE/Math/Vector.h>

#include <PGE/Math/Math.h>

using namespace PGE;

String Vector2f::toString() const {
    return "Vector2f(" + String::fromFloat(x) + ", " + String::fromFloat(y) + ")";
}

String Vector3f::toString() const {
    return "Vector3f(" + String::fromFloat(x) + ", " + String::fromFloat(y) + ", " + String::fromFloat(z) + ")";
}

String Vector4f::toString() const {
    return "Vector4f(" + String::fromFloat(x) + ", " + String::fromFloat(y) + ", " + String::fromFloat(z) + ", " + String::fromFloat(w) + ")";
}

String Vector2i::toString() const {
    return "Vector2i(" + String::fromInt(x) + ", " + String::fromInt(y) + ")";
}

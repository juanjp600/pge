#ifndef PGE_VECTOR_H_INCLUDED
#define PGE_VECTOR_H_INCLUDED

#include <PGE/String/String.h>
#include <PGE/Math/Math.h>

// If you modify one class here, apply your changes to the others as well!
namespace PGE {

class Vector2f : private NoHeap {
    public:
        float x; float y;

        constexpr Vector2f() : x(0.f), y(0.f) { }
        constexpr Vector2f(float s) : x(s), y(s) { }
        constexpr Vector2f(float ix, float iy) : x(ix), y(iy) { }

        constexpr bool operator==(const Vector2f& other) const { return x == other.x && y == other.y; }
        constexpr bool operator!=(const Vector2f& other) const { return x != other.x || y != other.y ; }

        constexpr bool equals(const Vector2f& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return Math::equalFloats(x, other.x, epsilon) && Math::equalFloats(y, other.y, epsilon);
        }

        constexpr const Vector2f operator-() const { return Vector2f(-x, -y); }

        constexpr Vector2f& operator+=(const Vector2f& other) { x += other.x; y += other.y; return *this; }
        constexpr Vector2f& operator-=(const Vector2f& other) { x -= other.x; y -= other.y; return *this; }
        constexpr Vector2f& operator+=(float f) { x += f; y += f; return *this; }
        constexpr Vector2f& operator-=(float f) { x -= f; y -= f; return *this; }
        constexpr Vector2f& operator*=(float f) { x *= f; y *= f; return *this; }
        constexpr Vector2f& operator/=(float f) { x /= f; y /= f; return *this; }

        constexpr const Vector2f operator+(const Vector2f& other) const { return Vector2f(x + other.x, y + other.y); }
        constexpr const Vector2f operator-(const Vector2f& other) const { return Vector2f(x - other.x, y - other.y); }
        constexpr const Vector2f operator+(float f) const { return Vector2f(x + f, y + f); }
        constexpr const Vector2f operator-(float f) const { return Vector2f(x - f, y - f); }
        constexpr const Vector2f operator*(float f) const { return Vector2f(x * f, y * f); }
        constexpr const Vector2f operator/(float f) const { return Vector2f(x / f, y / f); }

        constexpr float dotProduct(const Vector2f& other) const { return x * other.x + y * other.y; }
        constexpr const Vector2f entrywiseProduct(const Vector2f& other) const { return Vector2f(x * other.x, y * other.y); }

        constexpr const Vector2f normalize() const {
            float lenSqr = lengthSquared();
            if (Math::equalFloats(0.f, lenSqr)) { return Vector2f(); }
            if (Math::equalFloats(1.f, lenSqr)) { return *this; }
            return *this / sqrtf(lenSqr);
        }

        constexpr const Vector2f reflect(const Vector2f& normal) const {
            Vector2f reflectedVector = normalize();
            reflectedVector = normal * 2.f * reflectedVector.dotProduct(normal) - reflectedVector;
            return reflectedVector.normalize();
        }

        constexpr float lengthSquared() const { return x * x + y * y; }
        inline float length() const { return sqrtf(lengthSquared()); }

        constexpr float distanceSquared(const Vector2f& other) const { return (*this - other).lengthSquared(); }
        inline float distance(const Vector2f& other) const { return sqrtf(distanceSquared(other)); }
};
constexpr const Vector2f operator*(float f, const Vector2f& vec) { return vec * f; }

class Vector3f : private NoHeap {
    public:
        float x; float y; float z;

        constexpr Vector3f() : x(0.f), y(0.f), z(0.f) { }
        constexpr Vector3f(float s) : x(s), y(s), z(s) { }
        constexpr Vector3f(float ix, float iy, float iz) : x(ix), y(iy), z(iz) { }
        constexpr Vector3f(const Vector2f& v2, float iz) : x(v2.x), y(v2.y), z(iz) { }
        constexpr Vector3f(float ix, const Vector2f& v2) : x(ix), y(v2.x), z(v2.y) { }

        constexpr bool operator==(const Vector3f& other) const { return x == other.x && y == other.y && z == other.z; }
        constexpr bool operator!=(const Vector3f& other) const { return x != other.x || y != other.y || z != other.z; }
        
        constexpr bool equals(const Vector3f& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return Math::equalFloats(x, other.x, epsilon) && Math::equalFloats(y, other.y, epsilon) && Math::equalFloats(z, other.z, epsilon);
        }

        constexpr const Vector3f operator-() const { return Vector3f(-x, -y, -z); }

        constexpr Vector3f& operator+=(const Vector3f& other) { x += other.x; y += other.y; z += other.z; return *this; }
        constexpr Vector3f& operator-=(const Vector3f& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
        constexpr Vector3f& operator+=(float f) { x += f; y += f; z += f; return *this; }
        constexpr Vector3f& operator-=(float f) { x -= f; y -= f; z -= f; return *this; }
        constexpr Vector3f& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
        constexpr Vector3f& operator/=(float f) { x /= f; y /= f; z /= f; return *this; }

        constexpr const Vector3f operator+(const Vector3f& other) const { return Vector3f(x + other.x, y + other.y, z + other.z); }
        constexpr const Vector3f operator-(const Vector3f& other) const { return Vector3f(x - other.x, y - other.y, z - other.z); }
        constexpr const Vector3f operator+(float f) const { return Vector3f(x + f, y + f, z + f); }
        constexpr const Vector3f operator-(float f) const { return Vector3f(x - f, y - f, z - f); }
        constexpr const Vector3f operator*(float f) const { return Vector3f(x * f, y * f, z * f); }
        constexpr const Vector3f operator/(float f) const { return Vector3f(x / f, y / f, z / f); }

        constexpr float dotProduct(const Vector3f& other) const { return x * other.x + y * other.y + z * other.z; }
        constexpr const Vector3f entrywiseProduct(const Vector3f& other) const { return Vector3f(x * other.x, y * other.y, z * other.z); }
        constexpr const Vector3f crossProduct(const Vector3f& other) const { return Vector3f(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); }

        constexpr const Vector3f normalize() const {
            float lenSqr = lengthSquared();
            if (Math::equalFloats(0.f, lenSqr)) { return Vector3f(); }
            if (Math::equalFloats(1.f, lenSqr)) { return *this; }
            return *this / sqrtf(lenSqr);
        }

        constexpr const Vector3f reflect(const Vector3f& normal) const {
            Vector3f reflectedVector = normalize();
            reflectedVector = normal * 2.f * reflectedVector.dotProduct(normal) - reflectedVector;
            return reflectedVector.normalize();
        }

        constexpr float lengthSquared() const { return x * x + y * y + z * z; }
        inline float length() const { return sqrtf(lengthSquared()); }

        constexpr float distanceSquared(const Vector3f& other) const { return (*this - other).lengthSquared(); }
        inline float distance(const Vector3f& other) const { return sqrtf(distanceSquared(other)); }
};
constexpr const Vector3f operator*(float f, const Vector3f& vec) { return vec * f; }

class Vector4f : private NoHeap {
    public:
        float x; float y; float z; float w;

        constexpr Vector4f() : x(0.f), y(0.f), z(0.f), w(0.f) { }
        constexpr Vector4f(float s) : x(s), y(s), z(s), w(s) { }
        constexpr Vector4f(float ix,float iy,float iz,float iw) : x(ix), y(iy), z(iz), w(iw) { }
        constexpr Vector4f(const Vector3f& v3, float iw) : x(v3.x), y(v3.y), z(v3.z), w(iw) { }
        constexpr Vector4f(float ix, const Vector3f& v3) : x(ix), y(v3.x), z(v3.y), w(v3.z) { }
        constexpr Vector4f(const Vector2f& v2a, const Vector2f& v2b) : x(v2a.x), y(v2a.y), z(v2b.x), w(v2b.y) { }

        constexpr bool operator==(const Vector4f& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }
        constexpr bool operator!=(const Vector4f& other) const { return x != other.x || y != other.y || z != other.z || w != other.w; }

        constexpr bool equals(const Vector4f& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return Math::equalFloats(x, other.x, epsilon) && Math::equalFloats(y, other.y, epsilon) && Math::equalFloats(z, other.z, epsilon) && Math::equalFloats(w, other.w, epsilon);
        }

        constexpr const Vector4f operator-() const { return Vector4f(-x, -y, -z, -w); }

        constexpr Vector4f& operator+=(const Vector4f& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
        constexpr Vector4f& operator-=(const Vector4f& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
        constexpr Vector4f& operator+=(float f) { x += f; y += f; z += f; w += f; return *this; }
        constexpr Vector4f& operator-=(float f) { x -= f; y -= f; z -= f; w -= f; return *this; }
        constexpr Vector4f& operator*=(float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
        constexpr Vector4f& operator/=(float f) { x /= f; y /= f; z /= f; w /= f; return *this; }

        constexpr const Vector4f operator+(const Vector4f& other) const { return Vector4f(x + other.x, y + other.y, z + other.z, w + other.w); }
        constexpr const Vector4f operator-(const Vector4f& other) const { return Vector4f(x - other.x, y - other.y, z - other.z, w - other.w); }
        constexpr const Vector4f operator+(float f) const { return Vector4f(x + f, y + f, z + f, w + f); }
        constexpr const Vector4f operator-(float f) const { return Vector4f(x - f, y - f, z - f, w - f); }
        constexpr const Vector4f operator*(float f) const { return Vector4f(x * f, y * f, z * f, w * f); }
        constexpr const Vector4f operator/(float f) const { return Vector4f(x / f, y / f, z / f, w / f); }

        constexpr float dotProduct(const Vector4f& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }
        constexpr const Vector4f entrywiseProduct(const Vector4f& other) const { return Vector4f(x * other.x, y * other.y, z * other.z, w * other.w); }

        constexpr const Vector4f normalize() const {
            float lenSqr = lengthSquared();
            if (Math::equalFloats(0.f, lenSqr)) { return Vector4f(); }
            if (Math::equalFloats(1.f, lenSqr)) { return *this; }
            return *this / sqrtf(lenSqr);
        }

        constexpr const Vector4f reflect(const Vector4f& normal) const {
            Vector4f reflectedVector = normalize();
            reflectedVector = normal * 2.f * reflectedVector.dotProduct(normal) - reflectedVector;
            return reflectedVector.normalize();
        }

        constexpr float lengthSquared() const { return x * x + y * y + z * z + w * w; }
        inline float length() const { return sqrtf(lengthSquared()); }

        constexpr float distanceSquared(const Vector4f& other) const { return (*this - other).lengthSquared(); }
        inline float distance(const Vector4f& other) const { return sqrtf(distanceSquared(other)); }
};
constexpr const Vector4f operator*(float f, const Vector4f& vec) { return vec * f; }

class Vector2i : private NoHeap {
    public:
        int x; int y;

        constexpr Vector2i() : x(0), y(0) { }
        constexpr Vector2i(int s) : x(s), y(s) { }
        constexpr Vector2i(int ix, int iy) : x(ix), y(iy) { }
        constexpr explicit Vector2i(const Vector2f& vf) : x((int)vf.x), y((int)vf.y) { }

        constexpr operator const Vector2f() const { return Vector2f((float)x, (float)y); }

        constexpr bool operator==(const Vector2i& other) const { return x == other.x && y == other.y; }
        constexpr bool operator!=(const Vector2i& other) const { return x != other.x || y != other.y; }

        constexpr const Vector2i operator-() const { return Vector2i(-x, -y); }

        constexpr Vector2i& operator+=(const Vector2i& other) { x += other.x; y += other.y; return *this; }
        constexpr Vector2i& operator-=(const Vector2i& other) { x -= other.x; y -= other.y; return *this; }
        constexpr Vector2i& operator+=(int i) { x += i; y += i; return *this; }
        constexpr Vector2i& operator-=(int i) { x -= i; y -= i; return *this; }
        constexpr Vector2i& operator*=(int i) { x *= i; y *= i; return *this; }

        constexpr const Vector2i operator+(const Vector2i& other) const { return Vector2i(x + other.x, y + other.y); }
        constexpr const Vector2i operator-(const Vector2i& other) const { return Vector2i(x - other.x, y - other.y); }
        constexpr const Vector2i operator+(int i) const { return Vector2i(x + i, y + i); }
        constexpr const Vector2i operator-(int i) const { return Vector2i(x - i, y - i); }
        constexpr const Vector2i operator*(int i) const { return Vector2i(x * i, y * i); }

        constexpr int dotProduct(const Vector2i& other) { return x * other.x + y * other.y; }
        constexpr const Vector2i entrywiseProduct(const Vector2i& other) const { return Vector2i(x * other.x, y * other.y); }

        constexpr int lengthSquared() const { return x * x + y * y; }
        // Sacrificing vendor specific sqrt optimizations are not worth the constexpr here.
        // Fuck you, C++ standard!
        inline float length() const { return sqrtf((float)lengthSquared()); }

        constexpr int distanceSquared(const Vector2i& other) const { return ((*this) - other).lengthSquared(); }
        inline float distance(const Vector2i& other) const { return sqrtf((float)distanceSquared(other)); }
};
constexpr const Vector2i operator*(int i, const Vector2i& vec) { return vec * i; }

namespace Vectors {
    constexpr Vector2f ZERO2F(0.f, 0.f);
    constexpr Vector2f ONE2F(1.f, 1.f);

    constexpr Vector3f ZERO3F(0.f, 0.f, 0.f);
    constexpr Vector3f ONE3F(1.f, 1.f, 1.f);

    constexpr Vector4f ZERO4F(0.f, 0.f, 0.f, 0.f);
    constexpr Vector4f ONE4F(1.f, 1.f, 1.f, 1.f);

    constexpr Vector2i ZERO2I(0, 0);
    constexpr Vector2i ONE2I(1, 1);
}

}

#endif // PGE_VECTOR_H_INCLUDED

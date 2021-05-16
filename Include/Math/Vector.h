#ifndef PGE_VECTOR_H_INCLUDED
#define PGE_VECTOR_H_INCLUDED

#include <String/String.h>
#include <Math/Math.h>

// TODO: Increase feature parity, remove redundant methods in favor of overloads where applicable.
namespace PGE {

class Vector2i;

class Vector2f {
    public:
        float x; float y;

        Vector2f();
        Vector2f(float s);
        Vector2f(float ix,float iy);
        Vector2f(const Vector2i& vi);

        bool operator==(const Vector2f& other) const;
        bool operator!=(const Vector2f& other) const;

        void operator+=(const Vector2f& other);
        void operator+=(float f);
        void operator-=(const Vector2f& other);
        void operator-=(float f);
        void operator*=(float f);
        void operator/=(float f);
        
        Vector2f operator-() const;

        Vector2f operator+(const Vector2f& other) const;
        Vector2f operator+(float f) const;
        Vector2f operator-(const Vector2f& other) const;
        Vector2f operator-(float f) const;
        Vector2f operator*(float f) const;
        Vector2f operator/(float f) const;

        float lengthSquared() const;
        float length() const;

        float distanceSquared(const Vector2f& b) const;
        float distance(const Vector2f& b) const;

        bool equals(const Vector2f& b, float epsilon = Math::EPSILON_DEFAULT) const;

        Vector2f normalize() const;

        Vector2f reflect(const Vector2f& n) const;
        float dotProduct(const Vector2f& b) const;

        String toString() const;

        static Vector2f lerp(const Vector2f& oldValue, const Vector2f& newValue, float interpolation);

        static const Vector2f ZERO;
        static const Vector2f ONE;
};

class Vector3f {
    public:
        float x; float y; float z;

        Vector3f();
        Vector3f(float s);
        Vector3f(float ix,float iy,float iz);

        bool operator==(const Vector3f& other) const;
        bool operator!=(const Vector3f& other) const;

        void operator+=(const Vector3f& other);
        void operator+=(float f);
        void operator-=(const Vector3f& other);
        void operator-=(float f);
        void operator*=(float f);
        void operator/=(float f);
        
        Vector3f operator-() const;

        Vector3f operator+(const Vector3f& other) const;
        Vector3f operator+(float f) const;
        Vector3f operator-(const Vector3f& other) const;
        Vector3f operator-(float f) const;
        Vector3f operator*(float f) const;
        Vector3f operator/(float f) const;

        float lengthSquared() const;
        float length() const;

        float distanceSquared(const Vector3f& b) const;
        float distance(const Vector3f& b) const;

        Vector3f normalize() const;

        Vector3f reflect(const Vector3f& n) const;
        float dotProduct(const Vector3f& b) const;
        Vector3f crossProduct(const Vector3f& b) const;

        String toString() const;

        static Vector3f lerp(const Vector3f& oldValue, const Vector3f& newValue, float interpolation);

        static const Vector3f ZERO;
        static const Vector3f ONE;
};

class Vector4f {
    public:
        float x; float y; float z; float w;

        Vector4f();
        Vector4f(float s);
        Vector4f(float ix,float iy,float iz,float iw);
        Vector4f(const Vector3f& v3, float iw);

        static const Vector4f ZERO;
        static const Vector4f ONE;
};

class Vector2i {
    public:
        int x; int y;

        Vector2i();
        Vector2i(int s);
        Vector2i(int ix,int iy);
        Vector2i(const Vector2f& vf);

        bool operator==(const Vector2i& other) const;
        bool operator!=(const Vector2i& other) const;

        void operator+=(const Vector2i& other);
        void operator+=(int i);
        void operator-=(const Vector2i& other);
        void operator-=(int i);
        void operator*=(int i);

        Vector2i operator-() const;

        Vector2i operator+(const Vector2i& other) const;
        Vector2i operator+(int i) const;
        Vector2i operator-(const Vector2i& other) const;
        Vector2i operator-(int i) const;
        Vector2i operator*(int i) const;

        int lengthSquared() const;
        float length() const;

        int distanceSquared(const Vector2i& b) const;
        float distance(const Vector2i& b) const;

        static const Vector2i ZERO;
        static const Vector2i ONE;
};

}

#endif // PGE_VECTOR_H_INCLUDED

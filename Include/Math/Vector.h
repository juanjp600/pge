#ifndef PGE_VECTOR_H_INCLUDED
#define PGE_VECTOR_H_INCLUDED

namespace PGE {

struct Vector2f; struct Vector3f; struct Vector2i;

struct Vector2f {
    float x; float y;

    Vector2f();
    Vector2f(float s);
    Vector2f(float ix,float iy);
    Vector2f(const Vector2i& vi);

    float lengthSquared() const;
    float length() const;

    float distanceSquared(const Vector2f& b) const;
    float distance(const Vector2f& b) const;

    bool equals(const Vector2f& b,float epsilon=0.002f) const;

    Vector2f add(const Vector2f& b) const;
    Vector2f subtract(const Vector2f& b) const;

    Vector2f multiply(float s) const;
    Vector2f normalize() const;

    Vector2f reflect(const Vector2f& n) const;
    float dotProduct(const Vector2f& b) const;

    static const Vector2f zero;
    static const Vector2f one;
};

struct Vector3f {
    float x; float y; float z;

    Vector3f();
    Vector3f(float s);
    Vector3f(float ix,float iy,float iz);

    float lengthSquared() const;
    float length() const;

    float distanceSquared(const Vector3f& b) const;
    float distance(const Vector3f& b) const;

    Vector3f add(const Vector3f& b) const;
    Vector3f subtract(const Vector3f& b) const;

    Vector3f multiply(float s) const;
    Vector3f normalize() const;

    float dotProduct(const Vector3f& b) const;
    Vector3f crossProduct(const Vector3f& b) const;

    static const Vector3f zero;
    static const Vector3f one;
};

struct Vector4f {
    float x; float y; float z; float w;

    Vector4f();
    Vector4f(float s);
    Vector4f(float ix,float iy,float iz,float iw);

    static const Vector4f zero;
    static const Vector4f one;
};

struct Vector2i {
    int x; int y;

    Vector2i();
    Vector2i(int s);
    Vector2i(int ix,int iy);
    Vector2i(const Vector2f& vf);

    int lengthSquared() const;
    float length() const;

    int distanceSquared(const Vector2i& b) const;
    float distance(const Vector2i& b) const;

    Vector2i add(const Vector2i& b) const;
    Vector2i subtract(const Vector2i& b) const;

    Vector2i multiply(int s) const;

    static const Vector2i zero;
    static const Vector2i one;
};

}

#endif

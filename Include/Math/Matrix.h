#ifndef PGE_MATRIX_H_INCLUDED
#define PGE_MATRIX_H_INCLUDED

#include "Vector.h"

namespace PGE {

class Matrix4x4f {
    public:
        float elements[4][4];

        Matrix4x4f();
        Matrix4x4f(float aa,float ab,float ac,float ad,
                   float ba,float bb,float bc,float bd,
                   float ca,float cb,float cc,float cd,
                   float da,float db,float dc,float dd);

        void operator*=(const Matrix4x4f& other);
        Matrix4x4f operator*(const Matrix4x4f& other) const;

        bool operator==(const Matrix4x4f& other) const;
        bool operator!=(const Matrix4x4f& other) const;

        Matrix4x4f transpose() const;

        Vector4f transform(const Vector4f& other) const;
        Vector3f transform(const Vector3f& other) const;
        
        String toString() const;

        static Matrix4x4f translate(const Vector3f& position);
        static Matrix4x4f scale(const Vector3f& scale);
        static Matrix4x4f rotate(const Vector3f& rotation);

        static Matrix4x4f constructWorldMat(const Vector3f& position, const Vector3f& scale, const Vector3f& rotation);
        static Matrix4x4f constructViewMat(const Vector3f& position, const Vector3f& forwardVector, const Vector3f& upVector);
        Vector3f extractViewTarget() const;
        Vector3f extractViewUp() const;
        Vector3f extractViewPosition() const;
        static Matrix4x4f constructPerspectiveMat(float horizontalfov, float aspectRatio, float nearZ, float farZ);
        static Matrix4x4f constructOrthographicMat(float width, float height, float nearZ, float farZ);

        static const Matrix4x4f IDENTITY;
        static const Matrix4x4f ZERO;
};

}

#endif // PGE_MATRIX_H_INCLUDED

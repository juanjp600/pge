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

        Matrix4x4f transpose() const;

        Matrix4x4f product(const Matrix4x4f& other) const;
        Vector4f transform(const Vector4f& other) const;
        Vector3f transform(const Vector3f& other) const;

        static Matrix4x4f constructWorldMat(const Vector3f& position,const Vector3f& scale,const Vector3f& rotation);
        static Matrix4x4f constructViewMat(const Vector3f& position,const Vector3f& target,const Vector3f& upVector);
        static Matrix4x4f constructProjectionMat(float horizontalfov, float aspectRatio, float nearZ, float farZ);

        static const Matrix4x4f identity;
};

}

#endif

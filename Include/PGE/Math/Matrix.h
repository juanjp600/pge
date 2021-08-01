#ifndef PGE_MATRIX_H_INCLUDED
#define PGE_MATRIX_H_INCLUDED

#include "Vector.h"

namespace PGE {

class Matrix4x4f : private NoHeap {
    public:
        float elements[4][4];

        constexpr Matrix4x4f() : elements() { }
        constexpr Matrix4x4f(
                float aa,float ab,float ac,float ad,
                float ba,float bb,float bc,float bd,
                float ca,float cb,float cc,float cd,
                float da,float db,float dc,float dd
            ) : elements {
                aa, ab, ac, ad,
                ba, bb, bc, bd,
                ca, cb, cc, cd,
                da, db, dc, dd,
            } { }

        static constexpr const Matrix4x4f translate(const Vector3f& position) {
            return Matrix4x4f(
                1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                position.x, position.y, position.z, 1.f
            );
        }
        // TODO: Custom trigonometric function implementation?
        static inline const Matrix4x4f rotate(const Vector3f& rotation) {
            float sinPitch = sin(rotation.x);
            float sinYaw = sin(rotation.y);
            float sinRoll = sin(rotation.z);
            float cosPitch = cos(rotation.x);
            float cosYaw = cos(rotation.y);
            float cosRoll = cos(rotation.z);

            Matrix4x4f pitchMat = Matrix4x4f(
                1.f, 0.f, 0.f, 0.f,
                0.f, cosPitch, sinPitch, 0.f,
                0.f, -sinPitch, cosPitch, 0.f,
                0.f, 0.f, 0.f, 1.f
            );

            Matrix4x4f yawMat = Matrix4x4f(
                cosYaw, 0.f, -sinYaw, 0.f,
                0.f, 1.f, 0.f, 0.f,
                sinYaw, 0.f, cosYaw, 0.f,
                0.f, 0.f, 0.f, 1.f
            );

            Matrix4x4f rollMat = Matrix4x4f(
                cosRoll, sinRoll, 0.f, 0.f,
                -sinRoll, cosRoll, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                0.f, 0.f, 0.f, 1.f
            );

            return rollMat * pitchMat * yawMat;
        }

        static constexpr const Matrix4x4f scale(const Vector3f& scale) {
            return Matrix4x4f(
                scale.x, 0.f, 0.f, 0.f,
                0.f, scale.y, 0.f, 0.f,
                0.f, 0.f, scale.z, 0.f,
                0.f, 0.f, 0.f, 1.f
            );
        }

        static constexpr const Matrix4x4f constructWorldMat(const Vector3f& position, const Vector3f& rotation, const Vector3f& scale) {
            return Matrix4x4f::scale(scale) * Matrix4x4f::rotate(rotation) * Matrix4x4f::translate(position);
        }

        static constexpr const Matrix4x4f constructViewMat(const Vector3f& position, const Vector3f& forwardVector, const Vector3f& upVector) {
            Vector3f zAxis = -forwardVector;
            zAxis = zAxis.normalize();

            Vector3f xAxis = upVector.crossProduct(zAxis);
            xAxis = xAxis.normalize();

            Vector3f yAxis = zAxis.crossProduct(xAxis);

            return Matrix4x4f(
                xAxis.x, yAxis.x, zAxis.x, 0.f,
                xAxis.y, yAxis.y, zAxis.y, 0.f,
                xAxis.z, yAxis.z, zAxis.z, 0.f,
                -xAxis.dotProduct(position), -yAxis.dotProduct(position), -zAxis.dotProduct(position), 1.f
            );
        }

        static inline const Matrix4x4f constructPerspectiveMat(float horizontalfov, float aspectRatio, float nearZ, float farZ) {
            float rad = horizontalfov * 0.5f;
            float nad = cos(rad) / sin(rad);

            return Matrix4x4f(
                nad * (-1.0f / aspectRatio), 0.f, 0.f, 0.f,
                0.f, nad, 0.f, 0.f,
                0.f, 0.f, farZ / (nearZ - farZ), -1.f,
                0.f, 0.f, (nearZ * farZ / (nearZ - farZ)), 1.f
            );
        }

        static constexpr const Matrix4x4f constructOrthographicMat(float width, float height, float nearZ, float farZ) {
            return Matrix4x4f(
                -2.f / width, 0.f, 0.f, 0.f,
                0.f, 2.f / height, 0.f, 0.f,
                0.f, 0.f, -1.f / (nearZ - farZ), 0.f,
                0.f, 0.f, -farZ / (nearZ - farZ), 1.f
            );
        }

        constexpr float* operator[](int x) {
            return elements[x];
        }

        constexpr const float* operator[](int x) const {
            return elements[x];
        }

        constexpr bool operator==(const Matrix4x4f& other) const {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (elements[i][j] != other.elements[i][j]) {
                        return false;
                    }
                }
            }
            return true;
        }

        constexpr bool operator!=(const Matrix4x4f& other) const {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (elements[i][j] != other.elements[i][j]) {
                        return true;
                    }
                }
            }
            return false;
        }

        constexpr bool equals(const Matrix4x4f& other, float epsilon = Math::EPSILON_DEFAULT) const {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (Math::equalFloats(elements[i][j], other.elements[i][j], epsilon)) {
                        return true;
                    }
                }
            }
            return false;
        }

        constexpr Matrix4x4f& operator+=(const Matrix4x4f& other) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    this->elements[i][j] += other.elements[i][j];
                }
            }
            return *this;
        }

        constexpr Matrix4x4f& operator*=(const Matrix4x4f& other) {
            Matrix4x4f retVal;
            for (int i = 0; i < 4; i++) {
                for (int k = 0; k < 4; k++) {
                    for (int j = 0; j < 4; j++) {
                        retVal.elements[i][j] += elements[i][k] * other.elements[k][j];
                    }
                }
            }
            *this = retVal;
            return *this;
        }

        constexpr Matrix4x4f& operator*=(float scalar) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    this->elements[i][j] *= scalar;
                }
            }
            return *this;
        }

        constexpr const Matrix4x4f operator+(const Matrix4x4f& other) const {
            Matrix4x4f retVal = *this;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    retVal.elements[i][j] += other.elements[i][j];
                }
            }
            return retVal;
        }

        constexpr const Matrix4x4f operator*(const Matrix4x4f& other) const {
            Matrix4x4f retVal;
            for (int i = 0; i < 4; i++) {
                for (int k = 0; k < 4; k++) {
                    for (int j = 0; j < 4; j++) {
                        retVal.elements[i][j] += elements[i][k] * other.elements[k][j];
                    }
                }
            }
            return retVal;
        }

        constexpr const Matrix4x4f operator*(float scalar) const {
            Matrix4x4f retVal = *this;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    retVal.elements[i][j] *= scalar;
                }
            }
            return retVal;
        }

        constexpr const Vector4f operator*(const Vector4f& vec) const {
            Vector4f retVal;
            retVal.x = vec.x * elements[0][0] + vec.y * elements[1][0] + vec.z * elements[2][0] + vec.w * elements[3][0];
            retVal.y = vec.x * elements[0][1] + vec.y * elements[1][1] + vec.z * elements[2][1] + vec.w * elements[3][1];
            retVal.z = vec.x * elements[0][2] + vec.y * elements[1][2] + vec.z * elements[2][2] + vec.w * elements[3][2];
            retVal.w = vec.x * elements[0][3] + vec.y * elements[1][3] + vec.z * elements[2][3] + vec.w * elements[3][3];
            return retVal;
        }

        constexpr const Vector3f transform(const Vector3f& vec) const {
            Vector4f retVal(vec.x, vec.y, vec.z, 1.f);
            retVal = (*this) * retVal;
            return Vector3f(retVal.x, retVal.y, retVal.z);
        }

        constexpr const Vector4f transform(const Vector4f& vec) const {
            return (*this) * vec;
        }

        constexpr const Matrix4x4f transpose() const {
            Matrix4x4f retVal;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    retVal.elements[i][j] = elements[j][i];
                }
            }
            return retVal;
        }

        constexpr const Vector3f extractViewTarget() const {
            return Vector3f(-elements[0][2], -elements[1][2], -elements[2][2]);
        }

        constexpr const Vector3f extractViewUp() const {
            return Vector3f(elements[0][1], elements[1][1], elements[2][1]);
        }

        constexpr const Vector3f extractViewPosition() const {
            Vector3f xAxis = Vector3f(elements[0][0], elements[1][0], elements[2][0]);
            Vector3f yAxis = Vector3f(elements[0][1], elements[1][1], elements[2][1]);
            Vector3f zAxis = Vector3f(elements[0][2], elements[1][2], elements[2][2]);
            return -(xAxis * elements[3][0] + yAxis * elements[3][1] + zAxis * elements[3][2]);
        }
};

namespace Matrices {
    constexpr Matrix4x4f ZERO = Matrix4x4f();
    constexpr Matrix4x4f IDENTITY = Matrix4x4f(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    );
}

}

#endif // PGE_MATRIX_H_INCLUDED

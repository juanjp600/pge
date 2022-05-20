#ifndef PGE_MATRIX_H_INCLUDED
#define PGE_MATRIX_H_INCLUDED

#include <PGE/Types/Range.h>
#include "Vector.h"

namespace PGE {

/// Row-major 4x4 matrix.
class Matrix4x4f {
    private:
        float elements[4][4];

    public:
        constexpr Matrix4x4f() : elements() { }
        constexpr Matrix4x4f(
                float r0c0, float r0c1, float r0c2, float r0c3,
                float r1c0, float r1c1, float r1c2, float r1c3,
                float r2c0, float r2c1, float r2c2, float r2c3,
                float r3c0, float r3c1, float r3c2, float r3c3
            ) : elements {
                r0c0, r0c1, r0c2, r0c3,
                r1c0, r1c1, r1c2, r1c3,
                r2c0, r2c1, r2c2, r2c3,
                r3c0, r3c1, r3c2, r3c3,
            } { }
        // Interpreted as column vectors
        constexpr Matrix4x4f(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3, const Vector4f& v4) : elements {
            v1.x, v2.x, v3.x, v4.x,
            v1.y, v2.y, v3.y, v4.y,
            v1.z, v2.z, v3.z, v4.z,
            v1.w, v2.w, v3.w, v4.w
        } { }

        static constexpr Matrix4x4f translate(const Vector3f& position) {
            return Matrix4x4f(
                1.f, 0.f, 0.f, position.x,
                0.f, 1.f, 0.f, position.y,
                0.f, 0.f, 1.f, position.z,
                0.f, 0.f, 0.f, 1.f
            );
        }

        static constexpr Matrix4x4f rotatePitch(float rot) {
            float sinV = Math::sin(rot);
            float cosV = Math::cos(rot);
            return Matrix4x4f(
                1.f, 0.f, 0.f, 0.f,
                0.f, cosV, -sinV, 0.f,
                0.f, sinV, cosV, 0.f,
                0.f, 0.f, 0.f, 1.f
            );
        }

        static constexpr Matrix4x4f rotateYaw(float rot) {
            float sinV = Math::sin(rot);
            float cosV = Math::cos(rot);
            return Matrix4x4f(
                cosV, 0.f, sinV, 0.f,
                0.f, 1.f, 0.f, 0.f,
                -sinV, 0.f, cosV, 0.f,
                0.f, 0.f, 0.f, 1.f
            );
        }

        static constexpr Matrix4x4f rotateRoll(float rot) {
            float sinV = Math::sin(rot);
            float cosV = Math::cos(rot);
            return Matrix4x4f(
                cosV, -sinV, 0.f, 0.f,
                sinV, cosV, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                0.f, 0.f, 0.f, 1.f
            );
        }

        static constexpr Matrix4x4f rotate(const Vector3f& rotation) {
            return rotateYaw(rotation.y) *
                   rotatePitch(rotation.x) *
                   rotateRoll(rotation.z);
        }

        static constexpr Matrix4x4f scale(const Vector3f& scale) {
            return Matrix4x4f(
                scale.x, 0.f, 0.f, 0.f,
                0.f, scale.y, 0.f, 0.f,
                0.f, 0.f, scale.z, 0.f,
                0.f, 0.f, 0.f, 1.f
            );
        }

        static constexpr Matrix4x4f lookAt(const Vector3f& from, const Vector3f& to, const Vector3f& fixedDir = Vector3f(0.f, 1.f, 0.f)) {
            Vector3f forward = (from - to).normalize();
            Vector3f right = fixedDir.crossProduct(forward).normalize();
            Vector3f up = forward.crossProduct(right).normalize();
            return Matrix4x4f(
                Vector4f(right, 0.f), Vector4f(up, 0.f), Vector4f(forward, 0.f), Vector4f(0.f, 0.f, 0.f, 1.f)
            );
        }

        static constexpr Matrix4x4f constructWorldMat(const Vector3f& position, const Vector3f& rotation, const Vector3f& scale) {
            return Matrix4x4f::translate(position) * Matrix4x4f::rotate(rotation) * Matrix4x4f::scale(scale);
        }

        /// Constructs a view matrix. Returns the identity matrix when position=(0,0,0), forwardVector=(0,0,1), upVector=(0,1,0)
        static constexpr Matrix4x4f constructViewMat(const Vector3f& position, const Vector3f& forwardVector, const Vector3f& upVector) {
            Vector3f zAxis = forwardVector;
            zAxis = zAxis.normalize();

            Vector3f xAxis = upVector.crossProduct(zAxis);
            xAxis = xAxis.normalize();

            Vector3f yAxis = zAxis.crossProduct(xAxis);

            return Matrix4x4f(
                xAxis.x, xAxis.y, xAxis.z, -xAxis.dotProduct(position),
                yAxis.x, yAxis.y, yAxis.z, -yAxis.dotProduct(position),
                zAxis.x, zAxis.y, zAxis.z, -zAxis.dotProduct(position),
                0.f, 0.f, 0.f, 1.f
            );
        }

        /// Constructs a perspective projection matrix.
        /// Horizontal field of view must be provided in radians.
        static constexpr Matrix4x4f constructPerspectiveMat(float horizontalFovRad, float aspectRatio, float nearZ, float farZ) {
            float halfFov = horizontalFovRad * 0.5f;
            float cotanHalfFov = Math::cos(halfFov) / Math::sin(halfFov);

            return Matrix4x4f(
                cotanHalfFov / aspectRatio, 0.f, 0.f, 0.f,
                0.f, cotanHalfFov, 0.f, 0.f,
                0.f, 0.f, (farZ + 1) / (farZ - nearZ), -(nearZ * (farZ + 1) / (farZ - nearZ)),
                0.f, 0.f, 1.f, 1.f
            );
        }

        static constexpr Matrix4x4f constructOrthographicMat(float width, float height, float nearZ, float farZ) {
            return Matrix4x4f(
                -2.f / width, 0.f, 0.f, 0.f,
                0.f, 2.f / height, 0.f, 0.f,
                0.f, 0.f, -1.f / (nearZ - farZ), -farZ / (nearZ - farZ),
                0.f, 0.f, 0.f, 1.f
            );
        }

        constexpr float* operator[](int x) {
            return elements[x];
        }

        constexpr const float* operator[](int x) const {
            return elements[x];
        }

        constexpr bool operator==(const Matrix4x4f& other) const = default;

        constexpr bool equals(const Matrix4x4f& other, float epsilon = Math::EPSILON_DEFAULT) const {
            for (int i : Range(4)) {
                for (int j : Range(4)) {
                    if (Math::equalFloats(elements[i][j], other.elements[i][j], epsilon)) {
                        return true;
                    }
                }
            }
            return false;
        }

        constexpr void operator+=(const Matrix4x4f& other) {
            for (int i : Range(4)) {
                for (int j : Range(4)) {
                    elements[i][j] += other.elements[i][j];
                }
            }
        }

        constexpr void operator*=(const Matrix4x4f& other) {
            Matrix4x4f retVal;
            for (int i : Range(4)) {
                for (int k : Range(4)) {
                    for (int j : Range(4)) {
                        retVal.elements[i][j] += elements[i][k] * other.elements[k][j];
                    }
                }
            }
            *this = retVal;
        }

        constexpr void operator*=(float scalar) {
            for (int i : Range(4)) {
                for (int j : Range(4)) {
                    elements[i][j] *= scalar;
                }
            }
        }

        constexpr Matrix4x4f operator+(const Matrix4x4f& other) const {
            Matrix4x4f retVal = *this;
            for (int i : Range(4)) {
                for (int j : Range(4)) {
                    retVal.elements[i][j] += other.elements[i][j];
                }
            }
            return retVal;
        }

        constexpr Matrix4x4f operator*(const Matrix4x4f& other) const {
            Matrix4x4f retVal;
            for (int i : Range(4)) {
                for (int j : Range(4)) {
                    for (int k : Range(4)) {
                        retVal.elements[i][j] += elements[i][k] * other.elements[k][j];
                    }
                }
            }
            return retVal;
        }

        constexpr Matrix4x4f operator*(float scalar) const {
            Matrix4x4f retVal = *this;
            for (int i : Range(4)) {
                for (int j : Range(4)) {
                    retVal.elements[i][j] *= scalar;
                }
            }
            return retVal;
        }

        constexpr Vector4f operator*(const Vector4f& vec) const {
            Vector4f retVal;
            retVal.x = vec.x * elements[0][0] + vec.y * elements[0][1] + vec.z * elements[0][2] + vec.w * elements[0][3];
            retVal.y = vec.x * elements[1][0] + vec.y * elements[1][1] + vec.z * elements[1][2] + vec.w * elements[1][3];
            retVal.z = vec.x * elements[2][0] + vec.y * elements[2][1] + vec.z * elements[2][2] + vec.w * elements[2][3];
            retVal.w = vec.x * elements[3][0] + vec.y * elements[3][1] + vec.z * elements[3][2] + vec.w * elements[3][3];
            return retVal;
        }

        /// Returns the result of the following matrix-vector product:
        /// --                   --   -- --
        /// | r0c0 r0c1 r0c2 r0c3 |   | x |
        /// | r1c0 r1c1 r1c2 r1c3 | * | y |
        /// | r2c0 r2c1 r2c2 r2c3 |   | z |
        /// | r3c0 r3c1 r3c2 r3c3 |   | 1 |
        /// --                   --   -- --
        /// The w component of the result is discarded.
        constexpr Vector3f transform(const Vector3f& vec) const {
            Vector4f retVal(vec.x, vec.y, vec.z, 1.f);
            retVal = (*this) * retVal;
            return Vector3f(retVal.x, retVal.y, retVal.z);
        }

        /// Returns the result of the following matrix-vector product:
        /// --                   --   -- --
        /// | r0c0 r0c1 r0c2 r0c3 |   | x |
        /// | r1c0 r1c1 r1c2 r1c3 | * | y |
        /// | r2c0 r2c1 r2c2 r2c3 |   | z |
        /// | r3c0 r3c1 r3c2 r3c3 |   | w |
        /// --                   --   -- --
        constexpr Vector4f transform(const Vector4f& vec) const {
            return (*this) * vec;
        }

        constexpr Matrix4x4f transpose() const {
            Matrix4x4f retVal;
            for (int i : Range(4)) {
                for (int j : Range(4)) {
                    retVal.elements[i][j] = elements[j][i];
                }
            }
            return retVal;
        }

        constexpr Vector3f extractViewTarget() const {
            return Vector3f(-elements[0][2], -elements[1][2], -elements[2][2]);
        }

        constexpr Vector3f extractViewUp() const {
            return Vector3f(elements[0][1], elements[1][1], elements[2][1]);
        }

        constexpr Vector3f extractViewPosition() const {
            Vector3f xAxis = Vector3f(elements[0][0], elements[1][0], elements[2][0]);
            Vector3f yAxis = Vector3f(elements[0][1], elements[1][1], elements[2][1]);
            Vector3f zAxis = Vector3f(elements[0][2], elements[1][2], elements[2][2]);
            return -(xAxis * elements[3][0] + yAxis * elements[3][1] + zAxis * elements[3][2]);
        }
};
static_assert(sizeof(Matrix4x4f) == sizeof(float[4][4]));

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

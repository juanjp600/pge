#include <Math/Matrix.h>
#include <math.h>

using namespace PGE;

const Matrix4x4f Matrix4x4f::identity = Matrix4x4f(1,0,0,0,
                                                   0,1,0,0,
                                                   0,0,1,0,
                                                   0,0,0,1);

Matrix4x4f::Matrix4x4f() {
    *this = identity;
}

Matrix4x4f::Matrix4x4f(float aa,float ab,float ac,float ad,
                       float ba,float bb,float bc,float bd,
                       float ca,float cb,float cc,float cd,
                       float da,float db,float dc,float dd) {
    elements[0][0] = aa; elements[0][1] = ab; elements[0][2] = ac; elements[0][3] = ad;
    elements[1][0] = ba; elements[1][1] = bb; elements[1][2] = bc; elements[1][3] = bd;
    elements[2][0] = ca; elements[2][1] = cb; elements[2][2] = cc; elements[2][3] = cd;
    elements[3][0] = da; elements[3][1] = db; elements[3][2] = dc; elements[3][3] = dd;
}

Matrix4x4f Matrix4x4f::transpose() const {
    Matrix4x4f retVal;
    for (int i=0;i<4;i++) {
        for (int j=0;j<4;j++) {
            retVal.elements[i][j] = elements[j][i];
        }
    }
    return retVal;
}

Matrix4x4f Matrix4x4f::product(const Matrix4x4f& other) const {
    Matrix4x4f retVal;
    for (int i=0;i<4;i++) {
        for (int j=0;j<4;j++) {
            retVal.elements[i][j] = 0.f;
            for (int k=0;k<4;k++) {
                retVal.elements[i][j] += elements[i][k]*other.elements[k][j];
            }
        }
    }
    return retVal;
}

Matrix4x4f Matrix4x4f::constructWorldMat(const Vector3f& position,const Vector3f& scale,const Vector3f& rotation) {
    Matrix4x4f translationMat = Matrix4x4f(1.f,0.f,0.f,0.f,
                                           0.f,1.f,0.f,0.f,
                                           0.f,0.f,1.f,0.f,
                                           position.x,position.y,position.z,1.f);

    Matrix4x4f scaleMat = Matrix4x4f(scale.x,0.f,0.f,0.f,
                                     0.f,scale.y,0.f,0.f,
                                     0.f,0.f,scale.z,0.f,
                                     0.f,0.f,0.f,1.f);

    float sinRoll = sin(rotation.x);
    float sinPitch = sin(rotation.y);
    float sinYaw = sin(rotation.z);
    float cosRoll = cos(rotation.x);
    float cosPitch = cos(rotation.y);
    float cosYaw = cos(rotation.z);

    Matrix4x4f rotationMat = Matrix4x4f(cosPitch*cosYaw,cosPitch*sinYaw,-sinPitch,0.f,
                                        sinRoll*sinPitch*cosYaw-cosRoll*sinYaw,sinRoll*sinPitch*sinYaw+cosRoll*cosYaw,sinRoll*cosPitch,0.f,
                                        cosRoll*sinPitch*cosYaw+sinRoll*sinYaw,cosRoll*sinPitch*sinYaw-sinRoll*cosYaw,cosRoll*cosPitch,0.f,
                                        0.f,0.f,0.f,1.f);

    return scaleMat.product(rotationMat.product(translationMat));
}

Matrix4x4f Matrix4x4f::constructViewMat(const Vector3f& position,const Vector3f& target,const Vector3f& upVector) {
    Matrix4x4f newMat = identity;

    Vector3f zAxis = target.multiply(-1.f);
    zAxis = zAxis.normalize();

    Vector3f xAxis = upVector.crossProduct(zAxis);
    xAxis = xAxis.normalize();

    Vector3f yAxis = zAxis.crossProduct(xAxis);

    newMat.elements[0][0] = xAxis.x;
    newMat.elements[0][1] = yAxis.x;
    newMat.elements[0][2] = zAxis.x;
    newMat.elements[0][3] = 0.f;

    newMat.elements[1][0] = xAxis.y;
    newMat.elements[1][1] = yAxis.y;
    newMat.elements[1][2] = zAxis.y;
    newMat.elements[1][3] = 0;

    newMat.elements[2][0] = xAxis.z;
    newMat.elements[2][1] = yAxis.z;
    newMat.elements[2][2] = zAxis.z;
    newMat.elements[2][3] = 0;

    newMat.elements[3][0] = -xAxis.dotProduct(position);
    newMat.elements[3][1] = -yAxis.dotProduct(position);
    newMat.elements[3][2] = -zAxis.dotProduct(position);
    newMat.elements[3][3] = 1;

    return newMat;
}

#include <PGE/Math/Vector.h>

#include <PGE/Math/Math.h>

using namespace PGE;

const Vector2f Vector2f::ZERO = Vector2f(0.f,0.f);
const Vector2f Vector2f::ONE = Vector2f(1.f,1.f);

Vector2f::Vector2f() {
    x = 0.f; y = 0.f;
}

Vector2f::Vector2f(float s) {
    x = s; y = s;
}

Vector2f::Vector2f(float ix,float iy) {
    x = ix; y = iy;
}

Vector2f::Vector2f(const Vector2i& vi) {
    x = (float)vi.x; y = (float)vi.y;
}

bool Vector2f::operator==(const PGE::Vector2f& other) const {
    return x == other.x && y == other.y;
}

bool Vector2f::operator!=(const PGE::Vector2f& other) const {
    return x != other.x || y != other.y;
}

void Vector2f::operator+=(const Vector2f& other) {
    x += other.x;
    y += other.y;
}

void Vector2f::operator+=(float f) {
    x += f;
    y += f;
}

void Vector2f::operator-=(const Vector2f& other) {
    x -= other.x;
    y -= other.y;
}

void Vector2f::operator-=(float f) {
    x -= f;
    y -= f;
}

void Vector2f::operator*=(float f) {
    x *= f;
    y *= f;
}

void Vector2f::operator/=(float f) {
    x /= f;
    y /= f;
}

Vector2f Vector2f::operator-() const {
    return Vector2f(-x, -y);
}

Vector2f Vector2f::operator+(const Vector2f& other) const {
    return Vector2f(x + other.x, y + other.y);
}

Vector2f Vector2f::operator+(float f) const {
    return Vector2f(x + f, y + f);
}

Vector2f Vector2f::operator-(const Vector2f& other) const {
    return Vector2f(x - other.x, y - other.y);
}

Vector2f Vector2f::operator-(float f) const {
    return Vector2f(x - f, y - f);
}

Vector2f Vector2f::operator*(float f) const {
    return Vector2f(x * f, y * f);
}

Vector2f operator*(float f, const Vector2f& vec) {
    return vec * f;
}

Vector2f Vector2f::operator/(float f) const {
    return Vector2f(x / f, y / f);
}

float Vector2f::lengthSquared() const {
    return x*x+y*y;
}

float Vector2f::length() const {
    return (float)sqrt(x*x+y*y);
}

float Vector2f::distanceSquared(const Vector2f& b) const {
    return (*this - b).lengthSquared();
}

float Vector2f::distance(const Vector2f& b) const {
    return (*this - b).length();
}

bool Vector2f::equals(const Vector2f& b,float epsilon) const {
    return distanceSquared(b)<=(epsilon*epsilon);
}

Vector2f Vector2f::normalize() const {
    float lenSqr = lengthSquared();
    if (Math::equalFloats(0.f, lenSqr)) { return Vector2f::ZERO; }
    if (Math::equalFloats(1.f, lenSqr)) { return *this; }
    return *this/sqrt(lenSqr);
}

Vector2f Vector2f::reflect(const Vector2f& n) const {
    Vector2f reflectedVector = normalize();
    reflectedVector = n * 2.f * reflectedVector.dotProduct(n) - reflectedVector;
    return reflectedVector.normalize();
}

float Vector2f::dotProduct(const Vector2f& b) const {
    return x*b.x+y*b.y;
}

String Vector2f::toString() const {
    return "Vector2f(" + String::fromFloat(x) + ", " + String::fromFloat(y) + ")";
}

const Vector3f Vector3f::ZERO = Vector3f(0.f,0.f,0.f);
const Vector3f Vector3f::ONE = Vector3f(1.f,1.f,1.f);

Vector3f::Vector3f() {
    x=0.f; y=0.f; z=0.f;
}

Vector3f::Vector3f(float s) {
    x = s; y = s; z = s;
}

Vector3f::Vector3f(float ix,float iy,float iz) {
    x=ix; y=iy; z=iz;
}

void Vector3f::operator+=(const Vector3f& other) {
    x += other.x;
    y += other.y;
    z += other.z;
}

void Vector3f::operator+=(float f) {
    x += f;
    y += f;
    z += f;
}

void Vector3f::operator-=(const Vector3f& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

void Vector3f::operator-=(float f) {
    x -= f;
    y -= f;
    z -= f;
}

void Vector3f::operator*=(float f) {
    x *= f;
    y *= f;
    z *= f;
}

void Vector3f::operator/=(float f) {
    x /= f;
    y /= f;
    z /= f;
}

Vector3f Vector3f::operator-() const {
    return Vector3f(-x, -y, -z);
}

Vector3f Vector3f::operator+(const Vector3f& other) const {
    return Vector3f(x + other.x, y + other.y, z + other.z);
}

Vector3f Vector3f::operator+(float f) const {
    return Vector3f(x + f, y + f, z + f);
}

Vector3f Vector3f::operator-(const Vector3f& other) const {
    return Vector3f(x - other.x, y - other.y, z - other.z);
}

Vector3f Vector3f::operator-(float f) const {
    return Vector3f(x - f, y - f, z - f);
}

Vector3f Vector3f::operator*(float f) const {
    return Vector3f(x * f, y * f, z * f);
}

Vector3f operator*(float f, const Vector3f& vec) {
    return vec * f;
}

Vector3f Vector3f::operator/(float f) const {
    return Vector3f(x / f, y / f, z / f);
}

bool Vector3f::operator==(const Vector3f& other) const {
    return x == other.x && y == other.y && z == other.z;
}

bool Vector3f::operator!=(const Vector3f& other) const {
    return x != other.x || y != other.y || z != other.z;
}

float Vector3f::lengthSquared() const {
    return x*x+y*y+z*z;
}

float Vector3f::length() const {
    return (float)sqrt(x*x+y*y+z*z);
}

float Vector3f::distanceSquared(const Vector3f& b) const {
    return (*this - b).lengthSquared();
}

float Vector3f::distance(const Vector3f& b) const {
    return (*this - b).length();
}

Vector3f Vector3f::normalize() const {
    float lenSqr = lengthSquared();
    if (Math::equalFloats(0.f, lenSqr)) { return Vector3f::ZERO; }
    if (Math::equalFloats(1.f, lenSqr)) { return *this; }
    return *this/sqrt(lenSqr);
}

Vector3f Vector3f::reflect(const Vector3f& n) const {
    Vector3f reflectedVector = normalize();
    reflectedVector = n * 2.f * reflectedVector.dotProduct(n) - reflectedVector;
    return reflectedVector.normalize();
}

float Vector3f::dotProduct(const Vector3f& b) const {
    return x*b.x+y*b.y+z*b.z;
}

Vector3f Vector3f::crossProduct(const Vector3f& b) const {
    return Vector3f(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);
}

String Vector3f::toString() const {
    return "Vector3f(" + String::fromFloat(x) + ", " + String::fromFloat(y) + ", " + String::fromFloat(z) + ")";
}

Vector4f::Vector4f() {
    x = 0.f; y = 0.f; z = 0.f; w = 0.f;
}

Vector4f::Vector4f(float s) {
    x = s; y = s; z = s; w = s;
}

Vector4f::Vector4f(float ix, float iy, float iz, float iw) {
    x = ix; y = iy; z = iz; w = iw;
}

Vector4f::Vector4f(const Vector3f& v3, float iw) {
    x = v3.x; y = v3.y; z = v3.z; w = iw;
}

const Vector4f Vector4f::ZERO = Vector4f(0.f,0.f,0.f,0.f);
const Vector4f Vector4f::ONE = Vector4f(1.f,1.f,1.f,1.f);

const Vector2i Vector2i::ZERO = Vector2i(0,0);
const Vector2i Vector2i::ONE = Vector2i(1,1);

Vector2i::Vector2i() {
    x=0; y=0;
}

Vector2i::Vector2i(int s) {
    x = s; y = s;
}

Vector2i::Vector2i(int ix,int iy) {
    x = ix; y = iy;
}

Vector2i::Vector2i(const Vector2f& vf) {
    x = (int)vf.x; y = (int)vf.y;
}

bool Vector2i::operator==(const PGE::Vector2i& other) const {
    return x == other.x && y == other.y;
}

bool Vector2i::operator!=(const PGE::Vector2i& other) const {
    return x != other.x || y != other.y;
}

void Vector2i::operator+=(const Vector2i& other) {
    x += other.x;
    y += other.y;
}

void Vector2i::operator+=(int i) {
    x += i;
    y += i;
}

void Vector2i::operator-=(const Vector2i& other) {
    x -= other.x;
    y -= other.y;
}

void Vector2i::operator-=(int i) {
    x -= i;
    y -= i;
}

void Vector2i::operator*=(int i) {
    x *= i;
    y *= i;
}

Vector2i Vector2i::operator-() const {
    return Vector2i(-x, -y);
}

Vector2i Vector2i::operator+(const Vector2i& other) const {
    return Vector2i(x + other.x, y + other.y);
}

Vector2i Vector2i::operator+(int i) const {
    return Vector2i(x + i, y + i);
}

Vector2i Vector2i::operator-(const Vector2i& other) const {
    return Vector2i(x - other.x, y - other.y);
}

Vector2i Vector2i::operator-(int i) const {
    return Vector2i(x - i, y - i);
}

Vector2i Vector2i::operator*(int i) const {
    return Vector2i(x * i, y * i);
}

Vector2i operator*(int i, const Vector2i& vec) {
    return vec * i;
}

int Vector2i::lengthSquared() const {
    return x*x+y*y;
}

float Vector2i::length() const {
    return (float)sqrt((int)(x*x+y*y));
}

int Vector2i::distanceSquared(const Vector2i& b) const {
    return (*this - b).lengthSquared();
}

float Vector2i::distance(const Vector2i& b) const {
    return (*this - b).length();
}

String Vector2i::toString() const {
    return "Vector2i(" + String::fromInt(x) + ", " + String::fromInt(y) + ")";
}

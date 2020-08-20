#include <math.h>
#include <Math/Vector.h>

using namespace PGE;

const Vector2f Vector2f::zero = Vector2f(0.f,0.f);
const Vector2f Vector2f::one = Vector2f(1.f,1.f);

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

Vector2f& Vector2f::operator+=(const Vector2f& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2f& Vector2f::operator+=(float f) {
    x += f;
    y += f;
    return *this;
}

Vector2f& Vector2f::operator-=(const Vector2f& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2f& Vector2f::operator-=(float f) {
    x -= f;
    y -= f;
    return *this;
}

Vector2f& Vector2f::operator*=(float f) {
    x *= f;
    y *= f;
    return *this;
}

Vector2f& Vector2f::operator/=(float f) {
    x /= f;
    y /= f;
    return *this;
}

Vector2f& Vector2f::operator-() {
    x = -x;
    y = -y;
    return *this;
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

Vector2f Vector2f::operator/(float f) const {
    return Vector2f(x / f, y / f);
}

bool Vector2f::operator==(const PGE::Vector2f& other) const {
    return x == other.x && y == other.y;
}

bool Vector2f::operator!=(const PGE::Vector2f& other) const {
    return x != other.x || y != other.y;
}

float Vector2f::lengthSquared() const {
    return x*x+y*y;
}

float Vector2f::length() const {
    return (float)sqrt(x*x+y*y);
}

float Vector2f::distanceSquared(const Vector2f& b) const {
    return subtract(b).lengthSquared();
}

float Vector2f::distance(const Vector2f& b) const {
    return subtract(b).length();
}

bool Vector2f::equals(const Vector2f& b,float epsilon) const {
    return distanceSquared(b)<=(epsilon*epsilon);
}

Vector2f Vector2f::add(const Vector2f& b) const {
    return Vector2f(x+b.x,y+b.y);
}

Vector2f Vector2f::subtract(const Vector2f& b) const {
    return Vector2f(x-b.x,y-b.y);
}

Vector2f Vector2f::multiply(float s) const {
    return Vector2f(x*s,y*s);
}

Vector2f Vector2f::invert() const {
    return Vector2f(-x, -y);
}

Vector2f Vector2f::normalize() const {
    float lenSqr = lengthSquared();
    if (lenSqr < 0.0001f) { return Vector2f::zero; }
    if (lenSqr > 0.9999f && lenSqr < 1.0001f) { return *this; }
    return multiply(1.f/sqrt(lenSqr));
}

Vector2f Vector2f::reflect(const Vector2f& n) const {
    Vector2f reflectedVector = normalize();
    reflectedVector = n.multiply(2.f*reflectedVector.dotProduct(n)).subtract(reflectedVector);
    return reflectedVector.normalize();
}

float Vector2f::dotProduct(const Vector2f& b) const {
    return x*b.x+y*b.y;
}

const String Vector2f::toString() const {
    return "Vector2f(" + PGE::String(x) + ", " + y + ")";
}

Vector2f Vector2f::lerp(const Vector2f& oldValue, const Vector2f& newValue, float interpolation) {
    return oldValue.add(newValue.subtract(oldValue).multiply(interpolation));
}

const Vector3f Vector3f::zero = Vector3f(0.f,0.f,0.f);
const Vector3f Vector3f::one = Vector3f(1.f,1.f,1.f);

Vector3f::Vector3f() {
    x=0.f; y=0.f; z=0.f;
}

Vector3f::Vector3f(float s) {
    x = s; y = s; z = s;
}

Vector3f::Vector3f(float ix,float iy,float iz) {
    x=ix; y=iy; z=iz;
}

Vector3f& Vector3f::operator+=(const Vector3f& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3f& Vector3f::operator+=(float f) {
    x += f;
    y += f;
    z += f;
    return *this;
}

Vector3f& Vector3f::operator-=(const Vector3f& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3f& Vector3f::operator-=(float f) {
    x -= f;
    y -= f;
    z -= f;
    return *this;
}

Vector3f& Vector3f::operator*=(float f) {
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

Vector3f& Vector3f::operator/=(float f) {
    x /= f;
    y /= f;
    z /= f;
    return *this;
}

Vector3f& Vector3f::operator-() {
    x = -x;
    y = -y;
    z = -z;
    return *this;
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
    return subtract(b).lengthSquared();
}

float Vector3f::distance(const Vector3f& b) const {
    return subtract(b).length();
}

Vector3f Vector3f::add(const Vector3f& b) const {
    return Vector3f(x+b.x,y+b.y,z+b.z);
}

Vector3f Vector3f::subtract(const Vector3f& b) const {
    return Vector3f(x-b.x,y-b.y,z-b.z);
}

Vector3f Vector3f::multiply(float s) const {
    return Vector3f(x*s,y*s,z*s);
}

Vector3f Vector3f::invert() const {
    return Vector3f(-x, -y, -z);
}

Vector3f Vector3f::normalize() const {
    float lenSqr = lengthSquared();
    if (lenSqr < 0.0001f) { return Vector3f::zero; }
    if (lenSqr > 0.9999f && lenSqr < 1.0001f) { return *this; }
    return multiply(1.f/sqrt(lenSqr));
}

Vector3f Vector3f::reflect(const Vector3f& n) const {
    Vector3f reflectedVector = normalize();
    reflectedVector = n.multiply(2.f*reflectedVector.dotProduct(n)).subtract(reflectedVector);
    return reflectedVector.normalize();
}

float Vector3f::dotProduct(const Vector3f& b) const {
    return x*b.x+y*b.y+z*b.z;
}

Vector3f Vector3f::crossProduct(const Vector3f& b) const {
    return Vector3f(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);
}

const String Vector3f::toString() const {
    return "Vector3f(" + PGE::String(x) + ", " + y + ", " + z + ")";
}

Vector3f Vector3f::lerp(const Vector3f& oldValue, const Vector3f& newValue, float interpolation) {
    return oldValue.add(newValue.subtract(oldValue).multiply(interpolation));
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

Vector4f::Vector4f(PGE::Vector3f v3, float iw) {
    x = v3.x; y = v3.y; z = v3.z; w = iw;
}

const Vector4f Vector4f::zero = Vector4f(0.f,0.f,0.f,0.f);
const Vector4f Vector4f::one = Vector4f(1.f,1.f,1.f,1.f);

const Vector2i Vector2i::zero = Vector2i(0,0);
const Vector2i Vector2i::one = Vector2i(1,1);

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

int Vector2i::lengthSquared() const {
    return x*x+y*y;
}

float Vector2i::length() const {
    return (float)sqrt((int)(x*x+y*y));
}

int Vector2i::distanceSquared(const Vector2i& b) const {
    return subtract(b).lengthSquared();
}

float Vector2i::distance(const Vector2i& b) const {
    return subtract(b).length();
}

Vector2i Vector2i::add(const Vector2i& b) const {
    return Vector2i(x+b.x,y+b.y);
}

Vector2i Vector2i::subtract(const Vector2i& b) const {
    return Vector2i(x-b.x,y-b.y);
}

Vector2i Vector2i::multiply(int s) const {
    return Vector2i(x*s,y*s);
}

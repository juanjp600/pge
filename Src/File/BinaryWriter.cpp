#include <PGE/File/BinaryWriter.h>

#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Math/AABBox.h>
#include <PGE/Math/Line.h>
#include <PGE/Math/Plane.h>
#include <PGE/Math/Rectangle.h>

#include "../String/UnicodeHelper.h"

using namespace PGE;

BinaryWriter::BinaryWriter(const FilePath& file, bool append)
    : AbstractIO(file, std::ios::binary | (append ? std::ios::app : std::ios::trunc)) { }

template <typename T>
void BinaryWriter::write(const T& t) {
    stream.write((char*)&t, sizeof(T));
    validate();
}

#define PGE_IO_DEFAULT_SPEC(T) template void BinaryWriter::write(const T& val)
PGE_IO_DEFAULT_SPEC(byte);
PGE_IO_DEFAULT_SPEC(short);
PGE_IO_DEFAULT_SPEC(int);
PGE_IO_DEFAULT_SPEC(long);
PGE_IO_DEFAULT_SPEC(long long);
PGE_IO_DEFAULT_SPEC(unsigned short);
PGE_IO_DEFAULT_SPEC(unsigned int);
PGE_IO_DEFAULT_SPEC(unsigned long);
PGE_IO_DEFAULT_SPEC(unsigned long long);
PGE_IO_DEFAULT_SPEC(float);
PGE_IO_DEFAULT_SPEC(double);
PGE_IO_DEFAULT_SPEC(long double);

template<> void BinaryWriter::write(const char16& val) {
    char buf[4];
    byte len = Unicode::wCharToUtf8(val, buf);
    stream.write(buf, len);
    validate();
}

template<> void BinaryWriter::write(const String& val) {
    stream.write(val.cstr(), val.byteLength() + 1);
    validate();
}

template<> void BinaryWriter::write(const Vector2f& val) {
    write<float>(val.x); write<float>(val.y);
}

template<> void BinaryWriter::write(const Vector3f& val) {
    write<float>(val.x); write<float>(val.y); write<float>(val.z);
}

template<> void BinaryWriter::write(const Vector4f& val) {
    write<float>(val.x); write<float>(val.y); write<float>(val.z); write<float>(val.w);
}

template<> void BinaryWriter::write(const Vector2i& val) {
    write<int>(val.x); write<int>(val.y);
}

template<> void BinaryWriter::write(const Matrix4x4f& val) {
    writeBytes((byte*)val.elements, sizeof(float) * 4 * 4);
}

template<> void BinaryWriter::write(const AABBox& val) {
    write<Vector3f>(val.getMin()); write<Vector3f>(val.getMax());
}

template<> void BinaryWriter::write(const Line2f& val) {
    write<Vector2f>(val.pointA); write<Vector2f>(val.pointB);
}

template<> void BinaryWriter::write(const Line2i& val) {
    write<Vector2i>(val.pointA); write<Vector2i>(val.pointB);
}

template<> void BinaryWriter::write(const Line3f& val) {
    write<Vector3f>(val.pointA); write<Vector3f>(val.pointB);
}

template<> void BinaryWriter::write(const Plane& val) {
    write<float>(val.distanceFromOrigin); write<Vector3f>(val.normal);
}

template<> void BinaryWriter::write(const Rectanglef& val) {
    write<Vector2f>(val.topLeftCorner()); write<Vector2f>(val.bottomRightCorner());
}

template<> void BinaryWriter::write(const Rectanglei& val) {
    write<Vector2i>(val.topLeftCorner()); write<Vector2i>(val.bottomRightCorner());
}

// TODO: C++20 range.
void BinaryWriter::writeBytes(const byte* data, size_t amount) {
    stream.write((char*)data, amount);
    validate();
}

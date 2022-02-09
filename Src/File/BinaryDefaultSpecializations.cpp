#include <PGE/File/BinaryReader.h>
#include <PGE/File/BinaryWriter.h>

#include <PGE/Color/Color.h>
#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Math/AABBox.h>
#include <PGE/Math/Line.h>
#include <PGE/Math/Plane.h>
#include <PGE/Math/Rectangle.h>

using namespace PGE;

template <typename T>
void BinaryWriter::write(const T& t) {
    stream.write((char*)&t, sizeof(T));
    validate();
}

template <typename T>
bool BinaryReader::tryRead(T& out) {
    stream.read((char*)&out, sizeof(T));
    return stream.good();
}

#define PGE_IO_DEFAULT_SPEC(T) \
template bool BinaryReader::tryRead(T& out); \
template void BinaryWriter::write(const T& val)

PGE_IO_DEFAULT_SPEC(signed char);
PGE_IO_DEFAULT_SPEC(short);
PGE_IO_DEFAULT_SPEC(int);
PGE_IO_DEFAULT_SPEC(long);
PGE_IO_DEFAULT_SPEC(long long);
PGE_IO_DEFAULT_SPEC(unsigned char);
PGE_IO_DEFAULT_SPEC(unsigned short);
PGE_IO_DEFAULT_SPEC(unsigned int);
PGE_IO_DEFAULT_SPEC(unsigned long);
PGE_IO_DEFAULT_SPEC(unsigned long long);
PGE_IO_DEFAULT_SPEC(float);
PGE_IO_DEFAULT_SPEC(double);
PGE_IO_DEFAULT_SPEC(long double);
PGE_IO_DEFAULT_SPEC(Color);
PGE_IO_DEFAULT_SPEC(Vector2f);
PGE_IO_DEFAULT_SPEC(Vector3f);
PGE_IO_DEFAULT_SPEC(Vector4f);
PGE_IO_DEFAULT_SPEC(Vector2i);
PGE_IO_DEFAULT_SPEC(Matrix4x4f);
PGE_IO_DEFAULT_SPEC(AABBox);
PGE_IO_DEFAULT_SPEC(Line2f);
PGE_IO_DEFAULT_SPEC(Line3f);
PGE_IO_DEFAULT_SPEC(Line2i);
PGE_IO_DEFAULT_SPEC(Plane);
PGE_IO_DEFAULT_SPEC(Rectanglef);
PGE_IO_DEFAULT_SPEC(Rectanglei);

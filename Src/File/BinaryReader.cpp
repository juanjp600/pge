#include <PGE/File/BinaryReader.h>

#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Math/AABBox.h>
#include <PGE/Math/Line.h>
#include <PGE/Math/Plane.h>
#include <PGE/Math/Rectangle.h>

#include "../String/UnicodeHelper.h"

using namespace PGE;

BinaryReader::BinaryReader(const FilePath& file)
    : AbstractIO(file) { }

bool BinaryReader::endOfFile() const {
    return stream.eof();
}

template <typename T>
bool BinaryReader::tryRead(T& out) {
    stream.read((char*)&out, sizeof(T));
    return stream.good();
}

#define PGE_IO_DEFAULT_SPEC(T) template bool BinaryReader::tryRead(T& out)
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

template<> bool BinaryReader::tryRead(char16& out) {
    char buf[4];
    if (!tryRead<char>(buf[0])) { return false; }
    byte codepoint = Unicode::measureCodepoint(buf[0]);
    for (int i = 1; i < codepoint; i++) {
        if (!tryRead<char>(buf[i])) { return false; }
    }
    out = Unicode::utf8ToWChar(buf, codepoint);
    return true;
}

template<> bool BinaryReader::tryRead(String& out) {
    out = String();
    char16 ch;
    bool succ;
    while ((succ = tryRead<char16>(ch)) && ch != 0) {
        out += ch;
    }
    return succ;
}

void BinaryReader::readStringInto(String& ref) {
    String ret;
    PGE_ASSERT(tryRead<String>(ref), BAD_STREAM);
}

template<> bool BinaryReader::tryRead(Vector2f& out) {
    return tryRead<float>(out.x) && tryRead<float>(out.y);
}

template<> bool BinaryReader::tryRead(Vector3f& out) {
    return tryRead<float>(out.x) && tryRead<float>(out.y) && tryRead<float>(out.z);
}

template<> bool BinaryReader::tryRead(Vector4f& out) {
    return tryRead<float>(out.x) && tryRead<float>(out.y) && tryRead<float>(out.z) && tryRead<float>(out.w);
}

template<> bool BinaryReader::tryRead(Vector2i& out) {
    return tryRead<int>(out.x) && tryRead<int>(out.y);
}

template<> bool BinaryReader::tryRead(Matrix4x4f& out) {
    stream.read((char*)out.elements, sizeof(float) * 4 * 4);
    return stream.good();
}

template<> bool BinaryReader::tryRead(AABBox& out) {
    Vector3f temp;
    bool succ = tryRead<Vector3f>(temp);
    if (!succ) { return false; }
    out = AABBox(temp);
    succ = tryRead<Vector3f>(temp);
    if (!succ) { return false; }
    out.addPoint(temp);
    return true;
}

template<> bool BinaryReader::tryRead(Line2f& out) {
    return tryRead<Vector2f>(out.pointA) && tryRead<Vector2f>(out.pointB);
}

template<> bool BinaryReader::tryRead(Line2i& out) {
    return tryRead<Vector2i>(out.pointA) && tryRead<Vector2i>(out.pointB);
}

template<> bool BinaryReader::tryRead(Line3f& out) {
    return tryRead<Vector3f>(out.pointA) && tryRead<Vector3f>(out.pointB);
}

template<> bool BinaryReader::tryRead(Plane& out) {
    return tryRead<float>(out.distanceFromOrigin) && tryRead<Vector3f>(out.normal);
}

template<> bool BinaryReader::tryRead(Rectanglef& out) {
    Vector2f tl; Vector2f br;
    bool succ = tryRead<Vector2f>(tl) && tryRead<Vector2f>(br);
    out = Rectanglef(tl, br);
    return succ;
}

template<> bool BinaryReader::tryRead(Rectanglei& out) {
    Vector2i tl; Vector2i br;
    bool succ = tryRead<Vector2i>(tl) && tryRead<Vector2i>(br);
    out = Rectanglei(tl, br);
    return succ;
}

bool BinaryReader::tryReadBytes(size_t count, std::vector<byte>& out) {
    out.resize(count);
    stream.read((char*)out.data(), count);
    return stream.good();
}

const std::vector<byte> BinaryReader::readBytes(size_t count) {
    std::vector<byte> ret;
    PGE_ASSERT(tryReadBytes(count, ret), BAD_STREAM);
    return ret;
}

void BinaryReader::readBytesInto(size_t count, std::vector<byte>& out) {
    PGE_ASSERT(tryReadBytes(count, out), BAD_STREAM);
}


bool BinaryReader::trySkip(size_t length) {
    stream.ignore(length);
    return stream.good();
}

void BinaryReader::skip(size_t length) {
    PGE_ASSERT(trySkip(length), BAD_STREAM);
}

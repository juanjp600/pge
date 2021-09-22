#include <PGE/File/BinaryReader.h>

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

template<> bool BinaryReader::tryRead<char16>(char16& out) {
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

#pragma warning(push)
#pragma warning(disable: 4701)
template<> bool BinaryReader::tryRead(Vector2f& out) {
    float x; float y;
    bool succ = tryRead<float>(x) && tryRead<float>(y);
    out = Vector2f(x, y);
    return succ;
}

template<> bool BinaryReader::tryRead(Vector3f& out) {
    float x; float y; float z;
    bool succ = tryRead<float>(x) && tryRead<float>(y) && tryRead<float>(z);
    out = Vector3f(x, y, z);
    return succ;
}
#pragma warning(pop)

void BinaryReader::readStringInto(String& ref) {
    String ret;
    PGE_ASSERT(tryRead<String>(ref), BAD_STREAM);
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

#include <PGE/File/BinaryWriter.h>

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

template<> void BinaryWriter::write<char16>(const char16& val) {
    char buf[4];
    byte len = Unicode::wCharToUtf8(val, buf);
    stream.write(buf, len);
    validate();
}

template<> void BinaryWriter::write<String>(const String& val) {
    stream.write(val.cstr(), val.byteLength() + 1);
    validate();
}

template<> void BinaryWriter::write<Vector2f>(const Vector2f& val) {
    write<float>(val.x); write<float>(val.y);
}

template<> void BinaryWriter::write<Vector3f>(const Vector3f& val) {
    write<float>(val.x); write<float>(val.y); write<float>(val.z);
}

// TODO: C++20 range.
void BinaryWriter::writeBytes(const byte* data, size_t amount) {
    stream.write((char*)data, amount);
    validate();
}

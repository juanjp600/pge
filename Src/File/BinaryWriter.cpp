#include <PGE/File/BinaryWriter.h>

#include "../String/UnicodeHelper.h"

using namespace PGE;

BinaryWriter::BinaryWriter(const FilePath& file, bool append)
    : AbstractIO(file, std::ios::binary | (append ? std::ios::app : std::ios::trunc)) { }

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

// TODO: C++20 range.
void BinaryWriter::writeBytes(const byte* data, size_t amount) {
    stream.write((char*)data, amount);
    validate();
}

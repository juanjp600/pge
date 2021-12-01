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

void BinaryWriter::writeBytes(const std::span<byte>& data) {
    stream.write((char*)data.data(), data.size());
    validate();
}

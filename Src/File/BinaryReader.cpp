#include <PGE/File/BinaryReader.h>

#include "../String/UnicodeHelper.h"

namespace PGE {

BinaryReader::BinaryReader(const FilePath& file)
    : AbstractIO(file) { }

// Two strikes and you're out.
bool BinaryReader::checkEOF() noexcept {
    if (stream.eof()) {
        if (!eof) {
            eof = true;
            return true;
        } else {
            return false;
        }
    } else {
        return stream.good();
    }
}

bool BinaryReader::endOfFile() const noexcept {
    return eof;
}

template <typename T>
T BinaryReader::read() {
    T val;
    stream.read((char*)&val, sizeof(T));
    PGE_ASSERT(checkEOF(), BAD_STREAM);
    return val;
}

const std::vector<byte> BinaryReader::readBytes(int count) {
    std::vector<byte> bytes(count);
    stream.read((char*)bytes.data(), count);
    PGE_ASSERT(checkEOF(), BAD_STREAM);
    return bytes;
}

bool BinaryReader::readBoolean() {
    return readByte() != 0;
}

byte BinaryReader::readByte() {
    return read<byte>();
}

i32 BinaryReader::readInt32() {
    return read<i32>();
}

u32 BinaryReader::readUInt32() {
    return read<u32>();
}

float BinaryReader::readFloat() {
    return read<float>();
}

double BinaryReader::readDouble() {
    return read<double>();
}

char16 BinaryReader::readUTF8Char() {
    char buf[4];
    buf[0] = readByte();
    byte codepoint = Unicode::measureCodepoint(buf[0]);
    for (int i = 1; i < codepoint; i++) {
        buf[i] = readByte();
        PGE_ASSERT(!endOfFile(), "End of file reached");
    }
    return Unicode::utf8ToWChar(buf, codepoint);
}

const String BinaryReader::readNullTerminatedString() {
    String ret;
    readNullTerminatedString(ret);
    return ret;
}

void BinaryReader::readNullTerminatedString(String& dest) {
    char16 ch;
    while ((ch = readUTF8Char()) != 0) {
        dest += ch;
    }
}

const String BinaryReader::readFixedLengthString(int length) {
    String ret;
    readFixedLengthString(ret, length);
    return ret;
}

void BinaryReader::readFixedLengthString(String& dest, int length) {
    for (int i = 0; i < length; i++) {
        dest += readUTF8Char();
    }
}

const Vector2f BinaryReader::readVector2f() {
    float x = readFloat();
    if (endOfFile()) { return Vector2f(); }
    float y = readFloat();
    return Vector2f(x, y);
}

const Vector3f BinaryReader::readVector3f() {
    float x = readFloat();
    if (endOfFile()) { return Vector3f(); }
    float y = readFloat();
    if (endOfFile()) { return Vector3f(); }
    float z = readFloat();
    return Vector3f(x, y, z);
}

void BinaryReader::skip(int length) {
    stream.ignore(length);
    PGE_ASSERT(checkEOF(), BAD_STREAM);
}

}

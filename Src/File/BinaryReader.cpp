#include <PGE/File/BinaryReader.h>

#include "StreamUtil.h"

namespace PGE {

BinaryReader::BinaryReader(const FilePath& file) {
    StreamUtil::safeOpen(stream, file, std::ios::binary);
}

void BinaryReader::earlyClose() {
    StreamUtil::safeClose(stream);
}

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

template <class T>
T BinaryReader::read() {
    T val;
    stream.read((char*)&val, sizeof(T));
    PGE_ASSERT(checkEOF(), StreamUtil::BAD_STREAM);
    return val;
}

std::vector<byte> BinaryReader::readBytes(int count) {
    std::vector<byte> bytes(count);
    stream.read((char*)bytes.data(), count);
    PGE_ASSERT(checkEOF(), StreamUtil::BAD_STREAM);
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

const String BinaryReader::readNullTerminatedString() {
    std::vector<char> chars;
    char c;
    do {
        c = readByte();
        PGE_ASSERT(!endOfFile(), "End of file reached");
        chars.push_back(c);
    } while (c != 0);
    return String(chars.data());
}

const String BinaryReader::readFixedLengthString(int length) {
    std::vector<char> chars(length + 1);
    stream.read(chars.data(), length);
    PGE_ASSERT(checkEOF(), StreamUtil::BAD_STREAM);
    chars[length] = 0;
    return String(chars.data());
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
    PGE_ASSERT(checkEOF(), StreamUtil::BAD_STREAM);
}

}

#include <PGE/File/BinaryWriter.h>
#include <PGE/Exception/Exception.h>

#include "StreamUtil.h"

// TODO: Add more types to this and reader?

namespace PGE {

BinaryWriter::BinaryWriter(const FilePath& path, bool append) {
    stream.open(path.cstr(), std::ios::binary | (append ? std::ios::app : 0));
    PGE_ASSERT(stream.is_open(), "Could not open (file: \"" + path.str() + "\")");
}

void BinaryWriter::earlyClose() {
    stream.close();
    PGE_ASSERT(stream.good(), StreamUtil::BAD_STREAM);
}

void BinaryWriter::validateStream() {
    if (!stream.good()) {
        stream.close();
        throw PGE_CREATE_EX(StreamUtil::BAD_STREAM);
    }
}

template <class T>
void BinaryWriter::write(T t) {
    stream.write((char*)&t, sizeof(T));
    validateStream();
}

void BinaryWriter::writeBytes(byte* bytes, int count) {
    stream.write((char*)bytes, count);
    validateStream();
}

void BinaryWriter::writeBoolean(bool b) {
    write((byte)(b ? 1 : 0));
}

void BinaryWriter::writeByte(byte b) {
    write(b);
}

void BinaryWriter::writeInt32(i32 i) {
    write(i);
}

void BinaryWriter::writeUInt32(u32 u) {
    write(u);
}

void BinaryWriter::writeFloat(float f) {
    write(f);
}

void BinaryWriter::writeDouble(double d) {
    write(d);
}

void BinaryWriter::writeNullTerminatedString(const String& str) {
    stream.write(str.cstr(), str.byteLength() + 1);
    validateStream();
}

void BinaryWriter::writeFixedLengthString(const String& str) {
    stream.write(str.cstr(), str.byteLength());
    validateStream();
}

void BinaryWriter::writeVector2f(const Vector2f& vec) {
    writeFloat(vec.x); writeFloat(vec.y);
}

void BinaryWriter::writeVector3f(const Vector3f& vec) {
    writeFloat(vec.x); writeFloat(vec.y); writeFloat(vec.z);
}

}

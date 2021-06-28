#include <PGE/File/BinaryWriter.h>

#include "StreamUtil.h"

namespace PGE {

BinaryWriter::BinaryWriter(const FilePath& file, bool append) {
    StreamUtil::safeOpen(stream, file, std::ios::binary | (append ? std::ios::app : std::ios::trunc));
}

void BinaryWriter::earlyClose() {
    StreamUtil::safeClose(stream);
}

template <class T>
void BinaryWriter::write(T t) {
    stream.write((char*)&t, sizeof(T));
    StreamUtil::validate(stream);
}

void BinaryWriter::writeBytes(byte* bytes, int count) {
    stream.write((char*)bytes, count);
    StreamUtil::validate(stream);
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
    StreamUtil::validate(stream);
}

void BinaryWriter::writeFixedLengthString(const String& str) {
    stream.write(str.cstr(), str.byteLength());
    StreamUtil::validate(stream);
}

void BinaryWriter::writeVector2f(const Vector2f& vec) {
    writeFloat(vec.x); writeFloat(vec.y);
}

void BinaryWriter::writeVector3f(const Vector3f& vec) {
    writeFloat(vec.x); writeFloat(vec.y); writeFloat(vec.z);
}

}

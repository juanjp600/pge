#include <Misc/BinaryWriter.h>

// TODO: Add more types to this and reader?

namespace PGE {

BinaryWriter::BinaryWriter(const FilePath& file) {
    stream.open(file.cstr(), std::ios_base::binary);
    PGE_ASSERT(stream.is_open(), "Could not open file");
}

BinaryWriter::~BinaryWriter() {
    stream.close();
}

template <class T>
void BinaryWriter::write(T t) {
    stream.write((char*)&t, sizeof(T));
}

void BinaryWriter::writeBytes(byte* bytes, int count) {
    stream.write((char*)bytes, count);
}

void BinaryWriter::writeBoolean(bool b) {
    write(b ? 1 : 0);
}

void BinaryWriter::writeByte(byte b) {
    write(b);
}

void BinaryWriter::writeInt(int32_t i) {
    write(i);
}

void BinaryWriter::writeUInt(uint32_t u) {
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
}

void BinaryWriter::writeFixedLengthString(const String& str) {
    stream.write(str.cstr(), str.byteLength());
}

void BinaryWriter::writeVector2f(const Vector2f& vec) {
    writeFloat(vec.x); writeFloat(vec.y);
}

void BinaryWriter::writeVector3f(const Vector3f& vec) {
    writeFloat(vec.x); writeFloat(vec.y); writeFloat(vec.z);
}

}

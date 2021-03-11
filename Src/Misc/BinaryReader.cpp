#include <Misc/BinaryReader.h>

#include <Misc/FilePath.h>

namespace PGE {

BinaryReader::BinaryReader(const FilePath& file) {
    stream.open(file.cstr(), std::ios_base::in | std::ios_base::binary);
}

BinaryReader::~BinaryReader() {
    stream.close();
}

template <typename T>
T BinaryReader::read() {
    T val;
    stream.read((char*)&val, sizeof(T));
    return val;
}

uint8_t* BinaryReader::readBytes(int count) {
    uint8_t* bytes = new uint8_t[count];
    stream.read((char*)bytes, count);
    return bytes;
}

bool BinaryReader::readBoolean() {
    return readByte() != 0;
}

uint8_t BinaryReader::readByte() {
    return read<uint8_t>();
}

int32_t BinaryReader::readInt() {
    return read<int32_t>();
}

uint32_t BinaryReader::readUInt() {
    return read<uint32_t>();
}

float BinaryReader::readFloat() {
    return read<float>();
}

double BinaryReader::readDouble() {
    return read<double>();
}

String BinaryReader::readNullTerminatedString() {
    std::vector<char> chars;
    char c;
    while ((c = readByte()) != 0) {
        chars.push_back(c);
    }
    chars.push_back(0);
    return String(chars.data());
}

String BinaryReader::readFixedLengthString(int length) {
    char* chars = new char[length+1];
    for (int i = 0; i < length; i++) {
        chars[i] = readByte();
    }
    chars[length] = 0;
    String s = String(chars);
    delete[] chars;
    return s;
}

Vector2f BinaryReader::readVector2f() {
    float x = readFloat(); float y = readFloat();
    return Vector2f(x,y);
}

Vector3f BinaryReader::readVector3f() {
    float x = readFloat(); float y = readFloat(); float z = readFloat();
    return Vector3f(x,y,z);
}

void BinaryReader::skip(int length) {
    stream.ignore(length);
}

}

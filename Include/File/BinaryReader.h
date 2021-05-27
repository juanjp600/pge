#ifndef PGE_BINARY_READER_H_INCLUDED
#define PGE_BINARY_READER_H_INCLUDED

#include <fstream>

#include <File/FilePath.h>
#include <Math/Vector.h>

namespace PGE {

class BinaryReader {
    private:
        std::ifstream stream;

        template <class T>
        T read();

    public:
        BinaryReader(const FilePath& file);
        ~BinaryReader();

        byte readByte();
        byte* readBytes(int count);
        bool readBoolean();
        int32_t readInt();
        uint32_t readUInt();
        float readFloat();
        double readDouble();
        String readNullTerminatedString();
        String readFixedLengthString(int length);
        Vector2f readVector2f();
        Vector3f readVector3f();

        void skip(int length);
};

}

#endif // PGE_BINARY_READER_H_INCLUDED

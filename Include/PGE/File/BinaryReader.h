#ifndef PGE_BINARY_READER_H_INCLUDED
#define PGE_BINARY_READER_H_INCLUDED

#include <fstream>

#include <PGE/File/FilePath.h>
#include <PGE/Math/Vector.h>

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
        void readBytes(int count, std::vector<byte>& into);
        bool readBoolean();
        i32 readInt();
        u32 readUInt();
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

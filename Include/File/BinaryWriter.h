#ifndef PGE_BINARY_WRITER_H_INCLUDED
#define PGE_BINARY_WRITER_H_INCLUDED

#include <fstream>

#include <File/FilePath.h>
#include <Math/Vector.h>

namespace PGE {

class BinaryWriter {
    private:
        std::ofstream stream;

        template <class T>
        void write(T t);

    public:
        BinaryWriter(const FilePath& file);
        ~BinaryWriter();

        void writeByte(byte b);
        void writeBytes(byte* bytes, int count);
        void writeBoolean(bool b);
        void writeInt(int32_t i);
        void writeUInt(uint32_t u);
        void writeFloat(float f);
        void writeDouble(double d);
        void writeNullTerminatedString(const String& str);
        void writeFixedLengthString(const String& str);
        void writeVector2f(const Vector2f& vec);
        void writeVector3f(const Vector3f& vec);
};

}

#endif // PGE_BINARY_WRITER_H_INCLUDED

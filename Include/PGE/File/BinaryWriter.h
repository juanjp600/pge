#ifndef PGE_BINARY_WRITER_H_INCLUDED
#define PGE_BINARY_WRITER_H_INCLUDED

#include <fstream>

#include <PGE/File/FilePath.h>
#include <PGE/Math/Vector.h>

namespace PGE {

/// Utility to write binary data to a file.
/// @see BinaryReader
class BinaryWriter {
    private:
        std::ofstream stream;

        template <class T>
        void write(T t);

    public:
        /// Opens the file handle.
        BinaryWriter(const FilePath& path);
        /// Closes the file handle.
        ~BinaryWriter();

        /// Writes a byte to the file.
        void writeByte(byte b);
        /// Writes a given amount of bytes to the file.
        /// In order to be read again, the amount of bytes must be known, so it should either be constant, or stored with the byte data manually.
        void writeBytes(byte* bytes, int count);
        /// Writes byte representing a boolean.
        /// @param[in] b `false` is written as `0`, `true` as `1`.
        void writeBoolean(bool b);
        /// Writes 4 bytes representing a signed integer.
        void writeInt(i32 i);
        /// Writes 4 bytes representing an unsigned integer.
        void writeUInt(u32 u);
        /// Writes 4 bytes representing a 32-bit floating point number.
        void writeFloat(float f);
        /// Writes 4 bytes representing a 64-bit floating point number.
        void writeDouble(double d);
        /// Writes String::byteLength() amount of bytes from String::cstr() to the file, in addition to a null terminating byte.
        /// Encoded using UTF-8.
        /// @see FileWriter for more control over encoding.
        void writeNullTerminatedString(const String& str);
        /// Writes String::byteLength() amount of bytes from String::cstr() to a file, without a terminating null byte.
        /// Encoded using UTF-8.
        /// 
        /// In order to be read again, its byte length must be known, so it should either be constant, or stored together with the string manually.
        /// @see FileWriter for more control over encoding.
        void writeFixedLengthString(const String& str);
        /// Writes 2 floats to the file, representing a 2 component vector.
        /// 32-bit floating point numbers written in order x, y.
        void writeVector2f(const Vector2f& vec);
        /// Writes 3 floats to the file, representing a 3 component vector.
        /// 32-bit floating point numbers written in order x, y, z.
        void writeVector3f(const Vector3f& vec);
};

}

#endif // PGE_BINARY_WRITER_H_INCLUDED

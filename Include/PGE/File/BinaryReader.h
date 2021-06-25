#ifndef PGE_BINARY_READER_H_INCLUDED
#define PGE_BINARY_READER_H_INCLUDED

#include <fstream>

#include <PGE/File/FilePath.h>
#include <PGE/Math/Vector.h>

namespace PGE {

/// Utility to read binary data from a file.
/// @see BinaryWriter
class BinaryReader {
    private:
        std::ifstream stream;

        template <class T>
        T read();

    public:
        /// Opens the file handle.
        BinaryReader(const FilePath& file);
        /// Closes the file handle.
        ~BinaryReader();

        /// Reads a byte from the file.
        byte readByte();
        /// Reads a specified amount of bytes.
        std::vector<byte> readBytes(int count);
        /// Interprets a single byte as a boolean.
        /// @returns `false` if the read byte is `0`, `true` otherwise.
        bool readBoolean();
        /// Interprets 4 bytes as a signed integer.
        i32 readInt();
        /// Interprets 4 bytes as an unsigned integer.
        u32 readUInt();
        /// Interprets 4 bytes as a 32-bit floating point number.
        float readFloat();
        /// Interprets 8 bytes as a 64-bit floating point number.
        double readDouble();
        /// Reads bytes into a String until a terminating null byte is encountered.
        /// Interpreted as UTF-8.
        /// @see FileReader for more control over encoding.
        String readNullTerminatedString();
        /// Reads the given amount of bytes into a String.
        /// Interpreted as UTF-8.
        /// @see FileReader for more control over encoding.
        String readFixedLengthString(int length);
        /// Interprets two floats as a 2 component vector.
        /// 32-bit floating point numbers read in order x, y.
        Vector2f readVector2f();
        /// Interprets three floats as a 3 component vector.
        /// 32-bit floating point numbers read in order x, y, z.
        Vector3f readVector3f();

        /// Skips a given amount of bytes from the file, simply discarding them.
        /// @param[in] length The amount of bytes to skip.
        void skip(int length);
};

}

#endif // PGE_BINARY_READER_H_INCLUDED

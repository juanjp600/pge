#ifndef PGE_BINARY_READER_H_INCLUDED
#define PGE_BINARY_READER_H_INCLUDED

#include <PGE/File/AbstractIO.h>
#include <PGE/Math/Vector.h>

namespace PGE {

/// Utility to read binary data from a file.
/// @throws #PGE::Exception Any read operation can raise an exception if reading failed or the reader is in an invalid state.
/// @see #endOfFile
/// @see #PGE::BinaryWriter
/// @see #PGE::TextWriter
class BinaryReader : private AbstractIO<std::ifstream> {
    private:
        bool eof = false;

        bool checkEOF() noexcept;

        template <typename T>
        T read();

    public:
        using AbstractIO::earlyClose;

        /// Opens the file handle.
        /// @throws #PGE::Exception if the path is invalid or the file could not be opened.
        BinaryReader(const FilePath& file);

        /// Whether a previous function call attempted to read past the end of the file.
        /// The first call to read past the end of the file will cause this to return true thereafter.
        /// In that case undefined data will be returned, which is not to be used.
        /// Any further attempts at reading data will raise an exception.
        bool endOfFile() const noexcept;

        /// Reads a single byte from the file.
        byte readByte();
        /// Reads a specified amount of bytes.
        std::vector<byte> readBytes(int count);
        /// Interprets a single byte as a boolean.
        /// @returns `false` if the read byte is `0`, `true` otherwise.
        bool readBoolean();
        /// Interprets 4 bytes as a signed integer.
        i32 readInt32();
        /// Interprets 4 bytes as an unsigned integer.
        u32 readUInt32();
        /// Interprets 4 bytes as a 32-bit floating point number.
        float readFloat();
        /// Interprets 8 bytes as a 64-bit floating point number.
        double readDouble();
        /// Interprets between 1 and 4 bytes as an UTF-8 character.
        char16 readUTF8Char();
        /// Reads bytes into a String until a terminating null byte is encountered.
        /// Interpreted as UTF-8.
        /// @see #PGE::TextReader for more control over encoding.
        const String readNullTerminatedString();
        /// This can be used in combination with the property of strings to never contract their internal capacity in order to avoid
        /// unnecessary allocations.
        /// @see #readNullTerminatedString()
        void readNullTerminatedString(String& dest);
        /// Reads the given amount of characters into a String.
        /// Interpreted as UTF-8.
        /// @see #PGE::TextReader for more control over encoding.
        const String readFixedLengthString(int length);
        /// This can be used in combination with the property of strings to never contract their internal capacity in order to avoid
        /// unnecessary allocations.
        /// @see #readFixedLengthString()
        void readFixedLengthString(String& dest, int length);
        /// Interprets two floats as a 2 component vector.
        /// 32-bit floating point numbers read in order x, y.
        const Vector2f readVector2f();
        /// Interprets three floats as a 3 component vector.
        /// 32-bit floating point numbers read in order x, y, z.
        const Vector3f readVector3f();

        /// Skips a given amount of bytes from the file, simply discarding them.
        /// @param[in] length The amount of bytes to skip.
        void skip(int length);
};

}

#endif // PGE_BINARY_READER_H_INCLUDED

#ifndef PGE_BINARY_READER_H_INCLUDED
#define PGE_BINARY_READER_H_INCLUDED

#include <fstream>

#include <PGE/File/FilePath.h>
#include <PGE/Math/Vector.h>

namespace PGE {

/// Utility to read binary data from a file.
/// @throws #PGE::Exception Any read operation can raise an exception if reading failed or the reader is in an invalid state.
/// @see #endOfFile
/// @see #PGE::BinaryWriter
class BinaryReader {
    private:
        std::ifstream stream;
        bool eof = false;

        bool validateStream() noexcept;

        template <class T>
        T read();

    public:
        /// Opens the file handle.
        /// @throws #PGE::Exception if the path is invalid or the file could not be opened.
        BinaryReader(const FilePath& file);

        /// Closes the stream prematurely.
        /// Calling this is *not* necessary, the destructor will clean everything up appropriately,
        /// however, possible failure of the close operation will be swallowed in the destructor, while it will be thrown here.
        /// 
        /// Calling *any* further methods after attempting to close a reader (including attempting to close it again)
        /// will have no ill effects, but will cause an exception to be raised every call.
        /// @throws #PGE::Exception if closing fails.
        void earlyClose();

        /// Whether a previous function call attempted to read past the end of the file.
        /// The first call to read past the end of the file will cause #endOfFile to return true thereafter.
        /// In this case undefined data will be returned, which is not to be used.
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
        /// Reads bytes into a String until a terminating null byte is encountered.
        /// Interpreted as UTF-8.
        /// @see #PGE::FileReader for more control over encoding.
        const String readNullTerminatedString();
        /// Reads the given amount of bytes into a String.
        /// Interpreted as UTF-8.
        /// @see #PGE::FileReader for more control over encoding.
        const String readFixedLengthString(int length);
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

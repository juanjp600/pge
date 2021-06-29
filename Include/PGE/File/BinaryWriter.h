#ifndef PGE_BINARY_WRITER_H_INCLUDED
#define PGE_BINARY_WRITER_H_INCLUDED

#include <PGE/File/AbstractIO.h>
#include <PGE/Math/Vector.h>

namespace PGE {

// TODO: Add more types to this and reader?
/// Utility to write binary data to a file.
/// After any call raises an exception the stream will be closed, any further operations will raise an exception again.
/// 
/// @throws #PGE::Exception Any write operation can raise an exception if writing failed or the writer is in an invalid state.
/// @see #PGE::BinaryReader
/// @see #PGE::TextWriter
class BinaryWriter : private AbstractIO<std::ofstream> {
    private:
        template <class T>
        void write(T t);

    public:
        using AbstractIO::earlyClose;

        /// Opens the stream.
        /// @param[in] file The file to write to.
        /// @param[in] append Whether data should be appended to the file or it should be overwritten.
        /// @throws #PGE::Exception if the path is invalid or the file could not be opened.
        BinaryWriter(const FilePath& file, bool append = false);

        /// Writes a single byte to the file.
        void writeByte(byte b);
        /// Writes a given amount of bytes to the file.
        /// In order to be read again, the amount of bytes must be known, so it should either be constant, or stored with the byte data manually.
        void writeBytes(byte* bytes, int count);
        /// Writes byte representing a boolean.
        /// @param[in] b `false` is written as `0`, `true` as `1`.
        void writeBoolean(bool b);
        /// Writes 4 bytes representing a signed integer.
        void writeInt32(i32 i);
        /// Writes 4 bytes representing an unsigned integer.
        void writeUInt32(u32 u);
        /// Writes 4 bytes representing a 32-bit floating point number.
        void writeFloat(float f);
        /// Writes 4 bytes representing a 64-bit floating point number.
        void writeDouble(double d);
        /// Writes #PGE::String::byteLength amount of bytes from #PGE::String::cstr to the file, in addition to a null terminating byte.
        /// Encoded using UTF-8.
        /// @see #PGE::TextWriter for more control over encoding.
        void writeNullTerminatedString(const String& str);
        /// Writes #PGE::String::byteLength amount of bytes from #PGE::String::cstr to a file, without a terminating null byte.
        /// Encoded using UTF-8.
        /// 
        /// In order to be read again, its byte length must be known, so it should either be constant, or stored together with the string manually.
        /// @see #PGE::TextWriter for more control over encoding.
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

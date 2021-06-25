#ifndef PGE_BINARY_WRITER_H_INCLUDED
#define PGE_BINARY_WRITER_H_INCLUDED

#include <fstream>

#include <PGE/File/FilePath.h>
#include <PGE/Math/Vector.h>

namespace PGE {

/// Utility to write binary data to a file.
/// After any call raises an exception the stream will be closed, any further operations will raise an exception again.
/// 
/// @throws #PGE::Exception Any write operation can raise an exception if writing failed or the writer is in an invalid state.
/// @see #PGE::BinaryReader
class BinaryWriter {
    private:
        std::ofstream stream;

        template <class T>
        void write(T t);

        void validateStream();

    public:
        /// Opens the stream.
        /// @throws #PGE::Exception if the file could not be opened.
        /// @param[in] path The file to open for writing.
        /// @param[in] append Whether data should be appended to the file or it should be overwritten.
        BinaryWriter(const FilePath& path, bool append = false);

        /// Closes the stream prematurely.
        /// Calling this is *not* necessary, the destructor will clean everything up appropriately,
        /// however, possible failure of the close operation will be swallowed in the destructor, while it will be thrown here.
        /// 
        /// Calling *any* further methods after attempting to close a writer (including attempting to close it again)
        /// will have no ill effects, but will cause an exception to be raised every call.
        /// @throws #PGE::Exception if closing was not wholly successful.
        void earlyClose();

        /// Writes a single byte to the file.
        /// @throws #PGE::Exception if something unexpected happens.
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
        /// Writes #PGE::String::byteLength amount of bytes from #PGE::String::cstr to the file, in addition to a null terminating byte.
        /// Encoded using UTF-8.
        /// @see #PGE::FileWriter for more control over encoding.
        void writeNullTerminatedString(const String& str);
        /// Writes #PGE::String::byteLength amount of bytes from #PGE::String::cstr to a file, without a terminating null byte.
        /// Encoded using UTF-8.
        /// 
        /// In order to be read again, its byte length must be known, so it should either be constant, or stored together with the string manually.
        /// @see #PGE::FileWriter for more control over encoding.
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

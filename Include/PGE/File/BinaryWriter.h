#ifndef PGE_BINARY_WRITER_H_INCLUDED
#define PGE_BINARY_WRITER_H_INCLUDED

#include <PGE/File/AbstractIO.h>
#include <PGE/Math/Vector.h>

namespace PGE {

// TODO: Add more types to this and reader?
/// Utility to write binary data to a file.
/// 
/// In order to expand the capabilities of BinaryWriter the generic write method can be partially specialized
/// with the type(s) you wish to support. It's recommended to closely adhere to the specification and do things
/// as they're done in the library. You have access to a `stream` member, to which all your data is to be written. After
/// writing has concluded it is recommended to call the protected `validate` method in order to do proper error handling.
/// Specializations for other types can utilize preexisting specializations (e.g. a Vector2f is written by calling `write<float>` twice).\n
/// It is recommended to also provide a specialization for reading, if one is provided for writing.\n
/// Variation of functionality of existing types can be achieved by providing a thin wrapper around the object you wish to handle differently.
/// 
/// Example:
/// ```cpp
/// struct FixedLengthString { const String& str; };
/// template<> void BinaryWriter::write<FixedLengthString>(const FixedLengthString& val) {
///     // Regular String writer uses byteLength() + 1 to include the terminating null byte.
///     stream.write(val.str.cstr(), val.str.byteLength());
///     validate();
/// }
/// // ...
/// myBinaryWriter.write<FixedLengthString>({ "asd" });
/// ```
/// @see #PGE::BinaryReader
/// @see #PGE::TextWriter
class BinaryWriter : private AbstractIO<std::ofstream> {
    private:

    public:
        using AbstractIO::earlyClose;

        /// Opens the stream.
        /// @param[in] file The file to write to.
        /// @param[in] append Whether data should be appended to the file or it should be overwritten.
        /// @throws #PGE::Exception if the path is invalid or the file could not be opened.
        BinaryWriter(const FilePath& file, bool append = false);

        /// Writes a type T to file.
        /// By default the following types are supported:
        /// - Self explanatory: u8, u16, u32, u64, i8, i16, i32, i64, float, double, long double
        /// - char16: 1-4 bytes represented as a single UTF-8 character.
        /// - String: A null terminated String of UTF-8 characters.
        /// - Vector2f: Floats in order x, y.
        /// - Vector3f: Floats in order x, y, z.
        /// @throws Exception If any kind of error occurs, this will cause the stream to be closed.
        ///         In such a case, nothing is guaranteed to be written.
        template <typename T>
        void write(const T& val);

        /// Writes the given bytes to file.
        /// Adheres to the #write specification.
        /// 
        /// In order to be read again, the amount of bytes must be known, so it
        /// should either be constant, or stored along with the byte data manually.
        void writeBytes(const byte* data, size_t amount);
};

}

#endif // PGE_BINARY_WRITER_H_INCLUDED

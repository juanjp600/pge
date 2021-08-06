#ifndef PGE_BINARY_READER_H_INCLUDED
#define PGE_BINARY_READER_H_INCLUDED

#include <PGE/File/AbstractIO.h>
#include <PGE/Math/Vector.h>

namespace PGE {

/// Utility to read binary data from a file.
/// 
/// In order to expand the capabilities of BinaryReader the generic tryRead method can be partially specialized
/// with the type(s) you wish to support. It's recommended to closely adhere to the specification and do things
/// as they're done in the library. You have access to a `stream` member, from which all your data is to be read.\n
/// If the stream turns bad at any point during the reading process (`stream.good()` returns false), the method should return false.
/// Specializations for other types can utilize preexisting specializations (e.g. a Vector2f is read by calling `read<float>` twice).\n
/// It is recommended to also provide a specialization for writing, if one is provided for reading.\n
/// Variation of functionality of existing types can be achieved by providing a thin wrapper around the object you wish to handle differently.
/// (See BinaryReader for an example.)\n
/// Do not specialize read in itself, it simply relies on tryRead internally.
/// @see #PGE::BinaryWriter
/// @see #PGE::TextWriter
class BinaryReader : private AbstractIO<std::ifstream> {
    public:
        using AbstractIO::earlyClose;

        /// Opens the file handle.
        /// @throws #PGE::Exception if the path is invalid or the file could not be opened.
        BinaryReader(const FilePath& file);

        /// Whether a previous operation has attempted to read past the end of the file.
        /// @see https://en.cppreference.com/w/cpp/io/basic_ios/eof
        bool endOfFile() const;

        /// Attempts to read a type T from file.
        /// Reading can fail because some kind of IO error occured or the end of the file has been reached.
        /// #endOfFile can be used to determine which of the two scenarios is the case.
        /// 
        /// In case reading failed the contents of out are undefined (the object can safely be destructed though).
        /// 
        /// By default the following types are supported:
        /// - Self explanatory: u8, u16, u32, u64, i8, i16, i32, i64, float, double, long double
        /// - char16: 1-4 bytes interpreted as a single UTF-8 character.
        /// - String: A null terminated String of UTF-8 characters. Any content previously in the supplied String is overwritten.
        /// - Vector2f: Floats in order x, y.
        /// - Vector3f: Floats in order x, y, z.
        /// @returns Whether the reading succeeded.
        template <typename T>
        bool tryRead(T& out);

        // TODO: Global, #PGE::Exception or just Exception???
        /// Reads a type T from file.
        /// Throws when #tryRead would return false.
        /// @returns An object of type T, if it has been read successfully.
        /// @throws Exception if any kind of error occured.
        /// @see #tryRead
        template <typename T>
        const T read() {
            T val;
            PGE_ASSERT(tryRead<T>(val), BAD_STREAM);
            return val;
        }

        /// Behaves exactly as does #read<String>.
        /// This can be used in combination with the property of strings to never contract their internal capacity in order to avoid
        /// unnecessary allocations.
        /// 
        /// Any content previously in the supplied String is overwritten.
        /// @see #read
        void readStringInto(String& ref);

        /// Tries to read a specified amount of bytes.
        /// Adheres to the #tryRead specification.
        /// 
        /// The vector is cleared, but not shrunken.
        /// @see #tryRead
        bool tryReadBytes(size_t count, std::vector<byte>& out);
        /// Reads a specified amount of bytes.
        /// Adheres to the #read specification.
        /// @see #read
        const std::vector<byte> readBytes(size_t count);
        /// Behaves exactly as does #readBytes.
        /// This can be used in combination with the property of vectors to not contract their internal capacity automatically in order
        /// to avoid unnecessary allocations.
        /// 
        /// Any content previously in the supplied vector is overwritten.
        /// @see #readBytes
        void readBytesInto(size_t count, std::vector<byte>& ref);

        /// Tries to skip a given amount of bytes from the file, simply discarding them.
        /// Adheres to the #tryRead specification.
        /// @see #tryRead
        bool trySkip(size_t length);
        /// Skips a given amount of bytes from the file, simply discarding them.
        /// Adheres to the #read specification.
        /// @see #read
        void skip(size_t length);
};

}

#endif // PGE_BINARY_READER_H_INCLUDED

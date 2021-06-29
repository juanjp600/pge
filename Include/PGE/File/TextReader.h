#ifndef PGE_TEXTREADER_H_DEFINED
#define PGE_TEXTREADER_H_DEFINED

#include <PGE/File/AbstractIO.h>

namespace PGE {

/// Utility to read text in different encodings from a file.
/// @throws #PGE::Exception Any read operation can raise an exception if reading failed or the reader is in an invalid state.
/// @see #endOfFile
/// @see #PGE::TextWriter
/// @see #PGE::BinaryReader
class TextReader : private AbstractIO<std::ifstream> {
    public:
        using AbstractIO::earlyClose;

        enum class Encoding {
            ASCII,
            UTF8,
            UTF16LE,
            UTF16BE,
        };

        /// Opens the stream.
        /// @param[in] file The file to read from.
        /// @param[in] encoding Define the encoding to be used when reading, if a byte order mark is encountered it takes precedence over the
        ///                     provided encoding.
        /// @throws #PGE::Exception if the path is invalid or the file could not be opened.
        TextReader(const FilePath& file, Encoding encoding = Encoding::UTF8);

        /// Whether the end of file has been encountered.
        /// If #readLine is used the end of file will be marked after the last line has been read, all lines returned are considerd valid.
        /// 
        /// If #readChar is used the first call to read past the end of the file will mark the end of the file, and undefined data is returned,
        /// which is not to be used.
        /// 
        /// Any subsequent calls to any of the reading methods will raise an exception.
        bool endOfFile() const noexcept;

        /// Reads a line and returns it.
        /// `\n`, `\r`, `\n\r` and `\r\n` are considered to be line endings.
        const String readLine();
        /// Reads a line into the given string.
        /// This can be used in combination with the property of strings to never contract their internal capacity in order to avoid
        /// unnecessary allocations.
        void readLine(String& dest);
        /// Reads a singular char.
        /// Line endings are returned exactly as they appear in the file.
        /// 
        /// If #endOfFile returns true after a call to this, the last read char is considered undefined and is not to be used.
        wchar readChar();

    private:
        Encoding encoding;
        bool eof = false;

        void spitOut(wchar ch);

        void reportEOF();
};

}

#endif // PGE_TEXTREADER_H_DEFINED

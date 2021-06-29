#ifndef PGE_TEXTWRITER_H_DEFINED
#define PGE_TEXTWRITER_H_DEFINED

#include <PGE/File/AbstractIO.h>

namespace PGE {

// TODO: More encodings?
/// Utility to write text to a file.
/// 
/// Normalizes line endings.
/// 
/// After any call raises an exception the stream will be closed, any further operations will raise an exception again.
/// 
/// @throws #PGE::Exception Any write operation can raise an exception if writing failed or the writer is in an invalid state.
/// @see #PGE::TextReader
/// @see #PGE::BinaryWriter
class TextWriter : private AbstractIO<std::ofstream> {
	public:
		using AbstractIO::earlyClose;

        /// Opens the stream.
        /// @throws #PGE::Exception if the path is invalid or the file could not be opened.
		TextWriter(const FilePath& file);

        /// Writes a string to stream.
		void write(const String& content);
        /// Writes a string to stream and appends `\n`.
		void writeLine(const String& content);
};

}

#endif // PGE_TEXTWRITER_H_DEFINED

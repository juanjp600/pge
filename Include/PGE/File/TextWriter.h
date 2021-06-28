#ifndef PGE_TEXTWRITER_H_DEFINED
#define PGE_TEXTWRITER_H_DEFINED

#include <fstream>

#include <PGE/File/FilePath.h>

namespace PGE {

// TODO: More encodings?
/// Utility to write text to a file.
/// After any call raises an exception the stream will be closed, any further operations will raise an exception again.
/// 
/// @throws #PGE::Exception Any write operation can raise an exception if writing failed or the writer is in an invalid state.
/// @see #PGE::TextReader
/// @see #PGE::BinaryWriter
class TextWriter {
	private:
        std::ofstream stream;

	public:
        /// Opens the stream.
        /// @throws #PGE::Exception if the path is invalid or the file could not be opened.
		TextWriter(const FilePath& file);

        /// Closes the stream prematurely.
        /// Calling this is *not* necessary, the destructor will clean everything up appropriately.
        /// However, possible failure of the close operation will be swallowed in the destructor, while it will be thrown here.
        /// 
        /// Calling *any* further methods after attempting to close (including attempting to close again)
        /// will have no ill effects, but will cause an exception to be raised every call.
        /// @throws #PGE::Exception if closing was not wholly successful.
        void earlyClose();

        /// Writes a string to stream.
        /// @throws #PGE::Exception if closing fails.
		void write(const String& content);
        /// Writes a string to stream and appends '\\n'.
        /// @throws #PGE::Exception if closing fails.
		void writeLine(const String& content);
};

}

#endif // PGE_TEXTWRITER_H_DEFINED

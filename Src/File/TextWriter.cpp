#include <PGE/File/TextWriter.h>

#include "StreamUtil.h"

using namespace PGE;

TextWriter::TextWriter(const FilePath& file) {
	StreamUtil::safeOpen(stream, file);
}

void TextWriter::earlyClose() {
	StreamUtil::safeClose(stream);
}

void TextWriter::write(const String& content) {
	stream.write(content.cstr(), content.byteLength());
	StreamUtil::validate(stream);
}

void TextWriter::writeLine(const String& content) {
	write(content);
	stream.write("\n", 1);
}

#include <PGE/File/TextWriter.h>

using namespace PGE;

TextWriter::TextWriter(const FilePath& file)
	: AbstractIO(file, 0) { }

void TextWriter::write(const String& content) {
	stream.write(content.cstr(), content.byteLength());
	validate();
}

void TextWriter::writeLine(const String& content) {
	write(content);
	stream.write("\n", 1);
}

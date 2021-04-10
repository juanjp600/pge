#include <Misc/FileWriter.h>

using namespace PGE;

FileWriter::FileWriter(const FilePath& path) {
	stream.open(path.cstr(), std::ofstream::out | std::ofstream::trunc);
}

FileWriter::~FileWriter() {
	stream.close();
}

void FileWriter::write(const String& content) {
	stream.write(content.cstr(), content.byteLength());
}

void FileWriter::writeLine(const String& content) {
	write(content);
	stream.write("\n", 1);
}

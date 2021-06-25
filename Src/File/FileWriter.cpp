#include <PGE/File/FileWriter.h>

#include <PGE/Exception/Exception.h>

#include "StreamUtil.h"

using namespace PGE;

FileWriter::FileWriter(const FilePath& path) {
	stream.open(path.cstr(), std::ofstream::trunc);
	PGE_ASSERT(stream.is_open(), "Could not open (file: \"" + path.str() + "\")");
}

void FileWriter::write(const String& content) {
	stream.write(content.cstr(), content.byteLength());
	PGE_ASSERT_STREAM(stream);
}

void FileWriter::writeLine(const String& content) {
	write(content);
	PGE_ASSERT_STREAM(stream);
	stream.write("\n", 1);
	PGE_ASSERT_STREAM(stream);
}

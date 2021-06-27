#include <PGE/File/FileWriter.h>

#include <PGE/Exception/Exception.h>

#include "StreamUtil.h"

using namespace PGE;

FileWriter::FileWriter(const FilePath& file) {
	PGE_ASSERT(file.isValid(), StreamUtil::INVALID_FILEPATH);
	stream.open(file.cstr(), std::ofstream::trunc);
	PGE_ASSERT(stream.is_open(), "Could not open (file: \"" + file.str() + "\")");
}

void FileWriter::write(const String& content) {
	stream.write(content.cstr(), content.byteLength());
}

void FileWriter::writeLine(const String& content) {
	write(content);
	stream.write("\n", 1);
}

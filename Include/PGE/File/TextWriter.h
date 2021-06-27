#ifndef PGE_FILEWRITER_H_DEFINED
#define PGE_FILEWRITER_H_DEFINED

#include <fstream>

#include <PGE/File/FilePath.h>

namespace PGE {

class TextWriter {
	private:
        std::ofstream stream;

	public:
		TextWriter(const FilePath& file);

		void write(const String& content);
		void writeLine(const String& content);
};

}

#endif // PGE_FILEWRITER_H_DEFINED

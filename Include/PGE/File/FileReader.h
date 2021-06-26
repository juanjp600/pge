#ifndef PGE_FILEREADER_H_DEFINED
#define PGE_FILEREADER_H_DEFINED

#include <fstream>

#include <PGE/File/FilePath.h>

namespace PGE {

class FileReader {
    public:
        enum class Encoding {
            UTF8,
            UTF16LE,
            UTF16BE,
        };

        FileReader(const FilePath& file);

        bool eof() const;

        void readLine(String& dest);
        char32_t readChar();

    private:
        std::ifstream stream;
        Encoding encoding;

        void spitOut(char32_t ch);

        void reportEOF();
};

}

#endif // PGE_FILEREADER_H_DEFINED

#ifndef PGE_FILEREADER_H_DEFINED
#define PGE_FILEREADER_H_DEFINED

#include <fstream>

#include <PGE/File/FilePath.h>

namespace PGE {

class FileReader {
    public:
        enum class Encoding {
            ASCII,
            UTF8,
            UTF16LE,
            UTF16BE,
        };

        FileReader(const FilePath& file, Encoding encoding = Encoding::ASCII);

        bool eof() const;

        void readLine(String& dest);
        wchar readChar();

    private:
        std::ifstream stream;
        Encoding encoding;

        void spitOut(wchar ch);

        void reportEOF();
};

}

#endif // PGE_FILEREADER_H_DEFINED

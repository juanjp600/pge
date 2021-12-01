#include <PGE/File/TextReader.h>

#include "../String/UnicodeHelper.h"

using namespace PGE;

static const String INVALID_ENCODING("Invalid encoding (Don't cast ints to Encoding)");
static const String UNEXPECTED_EOF("Encountered an unexpected end of file");

TextReader::TextReader(const FilePath& file, Encoding enc)
    : AbstractIO(file) {
    // Try reading BOM.
    byte begin[3];
    int rewind;
    stream.read((char*)begin, 3);
    using enum Encoding;
    if (begin[0] == 0xEF && begin[1] == 0xBB && begin[2] == 0xBF) {
        encoding = UTF8;
        rewind = 0;
    } else if (begin[0] == 0xFF && begin[1] == 0xFE) {
        encoding = UTF16LE;
        rewind = 1;
    } else if (begin[0] == 0xFE && begin[1] == 0xFF) {
        encoding = UTF16BE;
        rewind = 1;
    } else {
        encoding = enc;
        rewind = 3;
    }
    for (int i = 0; i < rewind; i++) {
        stream.rdbuf()->sungetc();
    }
}

bool TextReader::endOfFile() const noexcept {
    return eof;
}

const String TextReader::readLine() {
    String str;
    readLine(str);
    return str;
}

void TextReader::readLine(String& dest) {
    // readChar takes care of checking for EOL.
    char16 ch = readChar();
    while (!eof && ch != L'\r' && ch != L'\n') {
        dest += ch;
        ch = readChar();
    }
    if (!eof) {
        // Pure carriage return linebreak are a thing!
        char16 checkChar = ch == L'\r' ? L'\n' : L'\r';
        // Eat the next character and spit it out if its not a continuaton of the EOL.
        if (checkChar != readChar()) {
            spitOut(checkChar);
        }
    }
}

char16 TextReader::readChar() {
    switch (encoding) {
        using enum Encoding;
        case ASCII: {
            int ch = stream.rdbuf()->sbumpc();
            if (ch == EOF) {
                reportEOF();
            }
            return (char16)ch;
        }
        case UTF8: {
            int ch = stream.rdbuf()->sbumpc();
            if (ch == EOF) {
                reportEOF();
                return (char16)ch;
            }
            int codepoint = Unicode::measureCodepoint((byte)ch);
            if (codepoint == 1) {
                return (char)ch;
            } else {
                char chs[3];
                chs[0] = (char)ch;
                for (int i = 1; i < codepoint; i++) {
                    int newCh = stream.rdbuf()->sbumpc();
                    if (newCh == EOF) {
                        reportEOF();
                        throw Exception(UNEXPECTED_EOF);
                    }
                    chs[i] = (char)newCh;
                }
                return Unicode::utf8ToWChar(chs, codepoint);
            }
        }
        case UTF16LE:
        case UTF16BE: {
            int ch = stream.rdbuf()->sbumpc();
            if (ch == EOF) {
                reportEOF();
                return (char16)ch;
            }
            int ch2 = stream.rdbuf()->sbumpc();
            if (ch2 == EOF) {
                reportEOF();
                throw Exception(UNEXPECTED_EOF);
            }
            if (encoding == UTF16LE) {
                return (char16)(ch | (ch2 << 8));
            } else {
                return (char16)((ch << 8) | ch2);
            }
        }
        default: {
            throw Exception(INVALID_ENCODING);
        }
    }
}

void TextReader::spitOut(char16 ch) {
    int backwards;
    switch (encoding) {
        using enum Encoding;
        case ASCII: {
            backwards = 1;
        } break;
        case UTF8: {
            backwards = Unicode::wCharToUtf8(ch, nullptr);
        } break;
        case UTF16BE:
        case UTF16LE: {
            backwards = 2;
        } break;
        default: {
            throw Exception(INVALID_ENCODING);
        }
    }
    for (int i = 0; i < backwards; i++) {
        // Fuck you, pubseekoff.
        stream.rdbuf()->sungetc();
    }
    // Clear EOF.
    eof = false;
}

void TextReader::reportEOF() {
    // Extraction failed, laugh at this reader!
    assert(!eof, "End of file has already been encountered");
    eof = true;
}

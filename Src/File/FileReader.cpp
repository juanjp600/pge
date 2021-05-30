#include <PGE/File/FileReader.h>

#include <PGE/Exception/Exception.h>

#include "../String/UnicodeHelper.h"

using namespace PGE;

FileReader::FileReader(const FilePath& file, Encoding enc) {
	stream.open(file.cstr());
	PGE_ASSERT(stream.is_open(), "Could not open (file: \"" + file.str() + "\")");

	// Try reading BOM.
	byte begin[3];
	int rewind;
	stream.read((char*)begin, 3);
	if (begin[0] == 0xEF && begin[1] == 0xBB && begin[2] == 0xBF) {
		encoding = Encoding::UTF8;
		rewind = 0;
	} else if (begin[0] == 0xFF && begin[1] == 0xFE) {
		encoding = Encoding::UTF16LE;
		rewind = 1;
	} else if (begin[0] == 0xFE && begin[1] == 0xFF) {
		encoding = Encoding::UTF16BE;
		rewind = 1;
	} else {
		encoding = Encoding::ASCII;
		rewind = 3;
	}
	for (int i = 0; i < rewind; i++) {
		stream.rdbuf()->sungetc();
	}
}

FileReader::~FileReader() {
	stream.close();
}

bool FileReader::eof() const {
	return stream.eof();
}

void FileReader::readLine(String& dest) {
	wchar ch = readChar();
	while (!eof() && ch != L'\r' && ch != L'\n') {
		dest += ch;
		ch = readChar();
	}
	if (!eof()) {
		// Pure carriage return linebreak are a thing!
		wchar checkChar = ch == L'\r' ? L'\n' : L'\r';
		// Eat the next character and spit it out if its not a continuaton of the EOL.
		if (checkChar != readChar()) {
			spitOut(checkChar);
		}
	}
}

wchar FileReader::readChar() {
	switch (encoding) {
		case Encoding::ASCII: {
			int ch = stream.rdbuf()->sbumpc();
			if (ch == EOF) {
				reportEOF();
			}
			return ch;
		}
		case Encoding::UTF8: {
			int ch = stream.rdbuf()->sbumpc();
			if (ch == EOF) {
				reportEOF();
				return ch;
			}
			int codepoint = Unicode::measureCodepoint(ch);
			if (codepoint == 1) {
				return (char)ch;
			} else {
				char chs[3];
				chs[0] = ch;
				for (int i = 1; i < codepoint; i++) {
					int newCh = stream.rdbuf()->sbumpc();
					if (newCh == EOF) {
						reportEOF();
						throw PGE_CREATE_EX("Unexpected EOF");
					}
					chs[i] = newCh;
				}
				return Unicode::utf8ToWChar(chs);
			}
		}
		case Encoding::UTF16LE:
		case Encoding::UTF16BE: {
			int ch = stream.rdbuf()->sbumpc();
			if (ch == EOF) {
				reportEOF();
				return ch;
			}
			int ch2 = stream.rdbuf()->sbumpc();
			if (ch2 == EOF) {
				reportEOF();
				throw PGE_CREATE_EX("Unexpected EOF");
			}
			if (encoding == Encoding::UTF16LE) {
				return (wchar)(ch | (ch2 << 8));
			} else {
				return (wchar)((ch << 8) | ch2);
			}
		}
	}
}

void FileReader::spitOut(wchar ch) {
	int backwards;
	switch (encoding) {
		case Encoding::ASCII: {
			backwards = 1;
		} break;
		case Encoding::UTF8: {
			backwards = Unicode::convertWCharToUtf8(ch, nullptr);
		} break;
		case Encoding::UTF16BE:
		case Encoding::UTF16LE: {
			backwards = 2;
		} break;
	}
	for (int i = 0; i < backwards; i++) {
		// Fuck you, pubseekoff.
		stream.rdbuf()->sungetc();
	}
	// Clear EOF.
	stream.setstate(std::ios::goodbit);
}

void FileReader::reportEOF() {
	// Extraction failed, laugh at this reader!
	stream.setstate(std::ios::eofbit | std::ios::failbit);
}

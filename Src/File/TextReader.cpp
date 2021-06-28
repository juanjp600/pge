#include <PGE/File/TextReader.h>

#include "../String/UnicodeHelper.h"
#include "StreamUtil.h"

using namespace PGE;

TextReader::TextReader(const FilePath& file, Encoding enc) {
	StreamUtil::safeOpen(stream, file, std::ios::binary);

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
		encoding = enc;
		rewind = 3;
	}
	for (int i = 0; i < rewind; i++) {
		stream.rdbuf()->sungetc();
	}
}

void TextReader::earlyClose() {
	StreamUtil::safeClose(stream);
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
	wchar ch = readChar();
	while (!eof && ch != L'\r' && ch != L'\n') {
		dest += ch;
		ch = readChar();
	}
	if (!eof) {
		// Pure carriage return linebreak are a thing!
		wchar checkChar = ch == L'\r' ? L'\n' : L'\r';
		// Eat the next character and spit it out if its not a continuaton of the EOL.
		if (checkChar != readChar()) {
			spitOut(checkChar);
		}
	}
}

static const String UNEXPECTED_EOF("Encountered an unexpected end of file");

wchar TextReader::readChar() {
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
						throw PGE_CREATE_EX(UNEXPECTED_EOF);
					}
					chs[i] = newCh;
				}
				return Unicode::utf8ToWChar(chs, codepoint);
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
				throw PGE_CREATE_EX(UNEXPECTED_EOF);
			}
			if (encoding == Encoding::UTF16LE) {
				return (wchar)(ch | (ch2 << 8));
			} else {
				return (wchar)((ch << 8) | ch2);
			}
		}
	}
}

void TextReader::spitOut(wchar ch) {
	int backwards;
	switch (encoding) {
		case Encoding::ASCII: {
			backwards = 1;
		} break;
		case Encoding::UTF8: {
			backwards = Unicode::wCharToUtf8(ch, nullptr);
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
	eof = false;
}

void TextReader::reportEOF() {
	// Extraction failed, laugh at this reader!
	PGE_ASSERT(!eof, "End of file has already been encountered");
	eof = true;
}

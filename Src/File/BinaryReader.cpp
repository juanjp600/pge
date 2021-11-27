#include <PGE/File/BinaryReader.h>

#include "../String/UnicodeHelper.h"

using namespace PGE;

BinaryReader::BinaryReader(const FilePath& file)
    : AbstractIO(file) { }

bool BinaryReader::endOfFile() const {
    return stream.eof();
}

template<> bool BinaryReader::tryRead(char16& out) {
    byte buf[4];
    if (!tryRead<byte>(buf[0])) { return false; }
    byte codepoint = Unicode::measureCodepoint(buf[0]);
    for (int i = 1; i < codepoint; i++) {
        if (!tryRead<byte>(buf[i])) { return false; }
    }
    out = Unicode::utf8ToWChar((char*)buf, codepoint);
    return true;
}

template<> bool BinaryReader::tryRead(String& out) {
    out = String();
    char16 ch;
    bool succ;
    while ((succ = tryRead<char16>(ch)) && ch != 0) {
        out += ch;
    }
    return succ;
}

void BinaryReader::readStringInto(String& ref) {
    String ret;
    PGE_ASSERT(tryRead<String>(ref), BAD_STREAM);
}

bool BinaryReader::tryReadBytes(size_t count, std::vector<byte>& out) {
    out.resize(count);
    stream.read((char*)out.data(), count);
    return stream.good();
}

const std::vector<byte> BinaryReader::readBytes(size_t count) {
    std::vector<byte> ret;
    PGE_ASSERT(tryReadBytes(count, ret), BAD_STREAM);
    return ret;
}

void BinaryReader::readBytesInto(size_t count, std::vector<byte>& out) {
    PGE_ASSERT(tryReadBytes(count, out), BAD_STREAM);
}


bool BinaryReader::trySkip(size_t length) {
    stream.ignore(length);
    return stream.good();
}

void BinaryReader::skip(size_t length) {
    PGE_ASSERT(trySkip(length), BAD_STREAM);
}

#ifndef PGE_INTERNAL_UNICODEHELPER_H_INCLUDED
#define PGE_INTERNAL_UNICODEHELPER_H_INCLUDED

#include <PGE/Types/Types.h>

namespace PGE {

namespace Unicode {
    byte measureCodepoint(unsigned char chr);
    char16 utf8ToWChar(const char* cbuffer);
    char16 utf8ToWChar(const char* cbuffer, int codepointLen);
    byte wCharToUtf8(char16 chr, char* result);
}

}

#endif // PGE_INTERNAL_UNICODEHELPER_H_INCLUDED

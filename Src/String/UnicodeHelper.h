#ifndef PGE_INTERNAL_UNICODEHELPER_H_INCLUDED
#define PGE_INTERNAL_UNICODEHELPER_H_INCLUDED

#include <PGE/Types/Types.h>

namespace PGE {

namespace Unicode {
    int measureCodepoint(unsigned char chr);
    char16 utf8ToWChar(const char* cbuffer);
    char16 utf8ToWChar(const char* cbuffer, int codepointLen);
    int wCharToUtf8(char16 chr, char* result);
}

}

#endif // PGE_INTERNAL_UNICODEHELPER_H_INCLUDED

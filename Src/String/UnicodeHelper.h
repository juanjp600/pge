#ifndef PGE_INTERNAL_UNICODEHELPER_H_INCLUDED
#define PGE_INTERNAL_UNICODEHELPER_H_INCLUDED

#include <PGE/String/String.h>

namespace PGE {

namespace Unicode {
    int measureCodepoint(unsigned char chr);
    wchar utf8ToWChar(const char* cbuffer);
    wchar utf8ToWChar(const char* cbuffer, int codepointLen);
    int wCharToUtf8(wchar chr, char* result);
}

}

#endif // PGE_INTERNAL_UNICODEHELPER_H_INCLUDED

#include "UnicodeHelper.h"

#include <PGE/Exception/Exception.h>

using namespace PGE;

static void assertChar(char16 chr, const std::source_location& location = std::source_location::current()) {
    asrt(chr != 0 && chr != 0xFFFF && chr != 0xFFFE, "Invalid character (" + String::hexFromInt<u16>(chr) + ")", location);
}

byte Unicode::measureCodepoint(byte chr) {
    if ((chr & 0x80) == 0x00) {
        // First bit is 0: treat as ASCII.
        return 1;
    }

    // First bit is 1: Number of consecutive 1 bits at the start is length of codepoint.
    byte len = 0;
    while (((chr >> (7 - len)) & 0x01) == 0x01) {
        len++;
    }
    return len;
}

char16 Unicode::utf8ToWChar(const char* cbuffer) {
    return utf8ToWChar(cbuffer, measureCodepoint(*cbuffer));
}

char16 Unicode::utf8ToWChar(const char* cbuffer, int codepointLen) {
    if (codepointLen == 1) {
        return cbuffer[0];
    } else {
        // Decode first byte by skipping all bits that indicate the length of the codepoint.
        char16 newChar = cbuffer[0] & (0x7f >> codepointLen);
        for (int j : Range(1, codepointLen)) {
            // Decode all of the following bytes, fixed 6 bits per byte.
            newChar = (newChar << 6) | (cbuffer[j] & 0x3f);
        }
        assertChar(newChar);
        return newChar;
    }
}


// TODO: Take into account UTF-16 surrogate pairs.
byte Unicode::wCharToUtf8(char16 chr, char* result) {
    assertChar(chr);

    // Fits in standard ASCII, just return the char as-is.
    if ((chr & 0x7f) == chr) {
        if (result != nullptr) { result[0] = (char)chr; }
        return 1;
    }

    byte len = 1;

    // Determine most of the bytes after the first one.
    while ((chr & (~0x3f)) != 0x00) {
        if (result != nullptr) { result[len - 1] = 0x80 | (chr & 0x3f); }
        chr >>= 6;
        len++;
    }

    // Determine the remaining byte(s): if the number of free bits in
    // the first byte isn't enough to fit the remaining bits,
    // add another byte.
    char firstByte = 0x00;
    for (int i : Range(len)) {
        firstByte |= (0x1 << (7 - i));
    }

    if (((firstByte | (0x1 << (7 - len))) & chr) == 0x00) {
        // It fits!
        firstByte |= chr;
    } else {
        // It doesn't fit: add another byte.
        if (result != nullptr) { result[len - 1] = 0x80 | (chr & 0x3f); }
        chr >>= 6;
        firstByte = (char)((firstByte | (0x1 << (7 - len))) | chr);
        len++;
    }

    if (result != nullptr) {
        result[len - 1] = firstByte;
        // Flip the result.
        for (int i : Range(len / 2)) {
            char b = result[i];
            result[i] = result[len - 1 - i];
            result[len - 1 - i] = b;
        }
    }

    return len;
}

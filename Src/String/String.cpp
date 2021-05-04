#include <String/String.h>
#include <String/Char.h>

#include <cstring>
#include <cwchar>
#include <cstdlib>
#include <cwctype>
#include <iostream>
#include <deque>
#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#endif

#include <Exception/Exception.h>

using namespace PGE;

static constexpr uint64_t FNV_SEED = 0xcbf29ce484222325;

//
// Utility
//

static int measureCodepoint(unsigned char chr) {
    if ((chr & 0x80) == 0x00) {
        //first bit is 0: treat as ASCII
        return 1;
    }

    //first bit is 1, number of consecutive 1 bits at the start is length of codepoint
    int len = 0;
    while (((chr >> (7 - len)) & 0x01) == 0x01) {
        len++;
    }
    return len;
}

static wchar utf8ToWChar(const char* cbuffer) {
    int codepointLen = measureCodepoint(cbuffer[0]);
    if (codepointLen == 1) {
        return cbuffer[0];
    } else {
        // Decode first byte by skipping all bits that indicate the length of the codepoint.
        wchar newChar = cbuffer[0] & (0x7f >> codepointLen);
        for (int j = 1; j < codepointLen; j++) {
            // Decode all of the following bytes, fixed 6 bits per byte.
            newChar = (newChar << 6) | (cbuffer[j] & 0x3f);
        }
        return newChar;
    }
}

// TODO: Take into account variable length codepoints.
static int convertWCharToUtf8(wchar chr, char* result) {
    // Fits in standard ASCII, just return the char as-is.
    if ((chr & 0x7f) == chr) {
        if (result != nullptr) { result[0] = (char)chr; }
        return 1;
    }

    int len = 1;

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
    for (int i = 0; i < len; i++) {
        firstByte |= (0x1 << (7 - i));
    }

    if (((firstByte | (0x1 << (7 - len))) & chr) == 0x00) {
        // It fits!
        firstByte |= chr;
    } else {
        // It doesn't fit: add another byte.
        if (result != nullptr) { result[len - 1] = 0x80 | (chr & 0x3f); }
        chr >>= 6;
        firstByte = (firstByte | (0x1 << (7 - len))) | chr;
        len++;
    }

    if (result != nullptr) {
        result[len - 1] = firstByte;
        // Flip the result.
        for (int i = 0; i < len / 2; i++) {
            char b = result[i];
            result[i] = result[len - 1 - i];
            result[len - 1 - i] = b;
        }
    }

    return len;
}

//

String::~String() {
    if (cCapacity > 0) {
        delete[] data.longStr;
    }
}

String::String() {
    reallocate(0);
    // Manual metadata:
    _strByteLength = 0;
    _strLength = 0;
    _hashCode = FNV_SEED;
    _hashCodeEvaluted = true;
    cstrNoConst()[0] = '\0';
}

String::String(const String& a) {
    reallocate(a.byteLength());
    _strByteLength = a._strByteLength;
    _strLength = a._strLength;
    _hashCodeEvaluted = a._hashCodeEvaluted;
    _hashCode = a._hashCode;
    memcpy(cstrNoConst(), a.cstr(), (a.byteLength() + 1) * sizeof(char));
}

String::String(const char* cstri) {
    int len = (int)strlen(cstri);
    reallocate(len);
    _strByteLength = len;
    memcpy(cstrNoConst(), cstri, (len + 1) * sizeof(char));
}

String::String(const std::string& cppstr) {
    int len = (int)cppstr.size();
    reallocate(len);
    _strByteLength = len;
    memcpy(cstrNoConst(), cppstr.c_str(), (len + 1) * sizeof(char));
}

String::String(const wchar* wstri) {
    wCharToUtf8Str(wstri);
}

String::String(const std::wstring& cppwstr) {
    wCharToUtf8Str(cppwstr.c_str());
}

void String::wCharToUtf8Str(const wchar* wbuffer) {
    // Determine the capacity of the cbuffer by measuring the number of bytes required for each codepoint.
    int newCap = 0;
    for (int i = 0; wbuffer[i] != L'\0'; i++) {
        newCap += convertWCharToUtf8(wbuffer[i], nullptr);
    }
    reallocate(newCap);

    // Convert all the wchars to codepoints.
    char* buf = cstrNoConst();
    int cIndex = 0;
    // We get _strLength "for free" here.
    for (_strLength = 0; wbuffer[_strLength] != L'\0'; _strLength++) {
        int increment = convertWCharToUtf8(wbuffer[_strLength], &buf[cIndex]);
        cIndex += increment;
    }
    buf[cIndex] = '\0';
    _strByteLength = cIndex;
}

#if defined(__APPLE__) && defined(__OBJC__)
String::String(const NSString* nsstr) {
    const char* cPath = [nsstr cStringUsingEncoding: NSUTF8StringEncoding];
    int len = (int)strlen(cPath);
    reallocate(len);
    _strByteLength = len;
    memcpy(cstrNoConst(), cPath, (len+1)*sizeof(char));
}
#endif

String::String(const String& a, const String& b) {
    int len = a.byteLength() + b.byteLength();
    reallocate(len);
    _strByteLength = len;
    char* buf = cstrNoConst();
    memcpy(buf, a.cstr(), a.byteLength());
    memcpy(buf + a.byteLength(), b.cstr(), b.byteLength());
    buf[len] = '\0';
}

String::String(char c) {
    char* buf = cstrNoConst();
    if (c < 0) {
        reallocate(2);
        _strByteLength = 2;
        convertWCharToUtf8((wchar)(unsigned char)c, buf);
        buf[2] = '\0';
    } else {
        reallocate(1);
        _strByteLength = 1;
        buf[0] = c;
        buf[1] = '\0';
    }
    _strLength = 1;
}

String::String(wchar w) {
    reallocate(4);
    char* buf = cstrNoConst();
    _strByteLength = convertWCharToUtf8(w, buf);
    _strLength = 1;
    buf[_strByteLength] = '\0';
}

//
// Private constructors.
//

String::String(int size) {
    reallocate(size);
}

// Byte substr.
String::String(const String& other, int from, int cnt) {
    reallocate(cnt);
    _strByteLength = cnt;
    char* buf = cstrNoConst();
    memcpy(buf, other.cstr() + from, cnt);
    buf[cnt] = '\0';
}

template <class T>
String String::format(T t, const String& format) {
    int size = snprintf(nullptr, 0, format.cstr(), t);
    String ret(size);
    sprintf(ret.cstrNoConst(), format.cstr(), t);
    ret._strByteLength = size;
    return ret;
}

//

template String String::format<int8_t>(int8_t t, const PGE::String& format);
template String String::format<int16_t>(int16_t t, const PGE::String& format);
template String String::format<int32_t>(int32_t t, const PGE::String& format);
template String String::format<int64_t>(int64_t t, const PGE::String& format);
template String String::format<uint8_t>(uint8_t t, const PGE::String& format);
template String String::format<uint16_t>(uint16_t t, const PGE::String& format);
template String String::format<uint32_t>(uint32_t t, const PGE::String& format);
template String String::format<uint64_t>(uint64_t t, const PGE::String& format);
template String String::format<float>(float t, const PGE::String& format);
template String String::format<double>(double t, const PGE::String& format);

String String::fromInt(int i) {
    // "-2147483648" has 11 characters.
    String ret(11);
    ret._strLength = sprintf(ret.cstrNoConst(), "%d", i);
    ret._strByteLength = ret._strLength;
    return ret;
}

String String::fromFloat(float f) {
    // Scientific notation to severly limit maximum output length.
    // sign + 6 * digits + point + e + expsign + 2 * expdigits
    String ret(12);
    ret._strLength = sprintf(ret.cstrNoConst(), "%g", f);
    ret._strByteLength = ret._strLength;
    return ret;
}

String& String::operator=(const String& other) {
    if (&other == this || other == *this) { return *this; }
    reallocate(other.byteLength());
    memcpy(cstrNoConst(), other.cstr(), (other.byteLength() + 1) * sizeof(char));
    _strByteLength = other._strByteLength;
    _strLength = other._strLength;
    _hashCodeEvaluted = other._hashCodeEvaluted;
    _hashCode = other._hashCode;
    return *this;
}

String& String::operator+=(const String& other) {
    *this = String(*this, other);
    return *this;
}

bool PGE::operator==(const String& a, const String& b) {
    return a.equals(b);
}

bool PGE::operator!=(const String& a, const String& b) {
    return !a.equals(b);
}

const String PGE::operator+(const String& a, const String& b) {
    return String(a, b);
}

const String PGE::operator+(const char* a, const String& b) {
    return String(String(a), b);
}

std::ostream& PGE::operator<<(std::ostream& os, const String& s) {
    return os.write(s.cstr(), s.byteLength());
}

uint64_t String::getHashCode() const {
    if (!_hashCodeEvaluted) {
        // FNV-1a
        // Public domain
        uint8_t* buf = (uint8_t*)cstr();
        _hashCode = FNV_SEED;
        // We get _strByteLength "for free" here.
        for (_strByteLength = 0; buf[_strByteLength] != '\0'; _strByteLength++) {
            _hashCode ^= buf[_strByteLength];
            _hashCode *= 0x00000100000001b3u;
        }
        _hashCodeEvaluted = true;
    }
    return _hashCode;
}

bool String::equals(const String& other) const {
    if (_strByteLength >= 0 && other._strByteLength >= 0 && byteLength() != other.byteLength()) { return false; }
    if (_strLength >= 0 && other._strLength >= 0 && length() != other.length()) { return false; }
    if (_hashCodeEvaluted && other._hashCodeEvaluted) { return getHashCode() == other.getHashCode(); }
    return strcmp(cstr(), other.cstr()) == 0;
}

static void fold(const char*& buf, std::deque<wchar>& queue) {
    if (queue.empty() && *buf != '\0') {
        Char::foldInto(utf8ToWChar(buf), queue);
        buf += measureCodepoint(*buf);
    }
}

bool String::equalsIgnoreCase(const String& other) const {
    if (_hashCodeEvaluted && other._hashCodeEvaluted && getHashCode() == other.getHashCode()) { return true; }

    const char* buf[2] = { cstr(), other.cstr() };
    std::deque<wchar> queue[2];

    // Feed first char.
    for (int i = 0; i < 2; i++) {
        fold(buf[i], queue[i]);
    }

    while (!queue[0].empty() && !queue[1].empty()) {
        if (queue[0].front() == queue[1].front()) {
            // Continue, are we done yet?
            queue[0].pop_front();
            queue[1].pop_front();
        } else {
            return false;
        }

        // Try refilling.
        for (int i = 0; i < 2; i++) {
            fold(buf[i], queue[i]);
        }
    }

    // If the strings are really equal, then both have the null char now.
    return *buf[0] == *buf[1];
}

bool String::isEmpty() const {
    return byteLength() == 0;
}

void String::reallocate(int byteLength) {
    // Invalidating metadata.
    _hashCodeEvaluted = false;
    _strLength = -1;
    _strByteLength = -1;

    // Accounting for the terminating byte.
    byteLength++;
    if (byteLength <= shortStrCapacity || byteLength <= cCapacity) { return; }
    int targetCapacity = 1;
    while (targetCapacity < byteLength) { targetCapacity <<= 1; }

    if (cCapacity > 0) {
        delete[] data.longStr;
    }
    cCapacity = targetCapacity;
    data.longStr = new char[targetCapacity];
}

const char* String::cstr() const {
    return cCapacity > 0 ? data.longStr : data.shortStr;
}

char* String::cstrNoConst() {
    return cCapacity > 0 ? data.longStr : data.shortStr;
}

void String::wstr(wchar* buffer) const {
    // Convert all the codepoints to wchars.
    const char* buf = cstr();
    int wIndex = 0;
    for (int i = 0; i < byteLength();) {
        buffer[wIndex] = utf8ToWChar(buf+i);

        i += measureCodepoint(buf[i]);
        wIndex++;
    }
    buffer[wIndex] = '\0';
}

int String::toInt(bool& success) const {
    try {
        success = true;
        return std::stoi(cstr());
    } catch (std::exception) {
        success = false;
        return 0;
    }
}

float String::toFloat(bool& success) const {
    try {
        success = true;
        return std::stof(cstr());
    } catch (std::exception) {
        success = false;
        return 0.f;
    }
}

int String::toInt() const {
    bool discard;
    return toInt(discard);
}

float String::toFloat() const {
    bool discard;
    return toFloat(discard);
}

int String::length() const {
    if (_strLength < 0) {
        const char* buf = cstr();
        _strLength = 0;
        // We get _strByteLength "for free" here.
        for (_strByteLength = 0; buf[_strByteLength] != '\0'; _strByteLength += measureCodepoint(buf[_strByteLength])) {
            _strLength++;
        }
    }
    return _strLength;
}

int String::byteLength() const {
    if (_strByteLength < 0) {
        _strByteLength = 0;
        const char* buf = cstr();
        while (buf[_strByteLength] != '\0') {
            _strByteLength++;
        }
    }
    return _strByteLength;
}

int String::findFirst(const String& fnd, int from) const {
    PGE_ASSERT(!fnd.isEmpty(), "Find string can't be empty");
    if (from < 0) { from = 0; }
    int charPos = 0;
    for (int i = 0; i <= byteLength() - fnd.byteLength(); i += measureCodepoint(cstr()[i])) {
        if (charPos >= from) {
            if (memcmp(fnd.cstr(), cstr() + i, fnd.byteLength()) == 0) { return charPos; }
        }
        charPos++;
    }
    return -1;
}

int String::findLast(const String& fnd, int from) const {
    PGE_ASSERT(!fnd.isEmpty(), "Find string can't be empty");
    if (from < 0) { from = 0; }
    const char* buf = cstr();
    int charPos = 0;
    int foundPos = -1;
    for (int i = 0; i <= byteLength() - fnd.byteLength(); i += measureCodepoint(buf[i])) {
        if (charPos >= from) {
            if (memcmp(fnd.cstr(), buf + i, fnd.byteLength()) == 0) { foundPos = charPos; }
        }
        charPos++;
    }
    return foundPos;
}

String String::substr(int start, int cnt) const {
    const char* buf = cstr();

    int startPos = 0;
    for (int i = 0; i < start; i++) {
        PGE_ASSERT(buf[startPos] != '\0', "Substring would go past string data (start: " + fromInt(start) + "; str: " + *this + ")");
        startPos += measureCodepoint(buf[startPos]);
    }

    int actualSize = 0;
    if (cnt < 0) {
        cnt = 0;
        while (buf[startPos + actualSize] != '\0') {
            actualSize += measureCodepoint(buf[startPos + actualSize]);
            cnt++;
        }
        // We get it for free here.
        _strLength = start + cnt;
    } else {
        for (int i = 0; i < cnt; i++) {
            PGE_ASSERT(buf[startPos + actualSize] != '\0', "Substring would go past string data (start: " + fromInt(start) + "; content: " + fromInt(cnt) + "; str: " + *this + ")");
            actualSize += measureCodepoint(buf[startPos + actualSize]);
        }
    }

    String retVal(actualSize);
    retVal._strByteLength = actualSize;
    retVal._strLength = cnt;
    char* retBuf = retVal.cstrNoConst();
    memcpy(retBuf, buf + startPos, actualSize);
    retBuf[actualSize] = '\0';
    return retVal;
}

wchar String::charAt(int pos) const {
    const char* buf = cstr();
    int strPos = 0;
    for (int i = 0; i < pos; i++) {
        strPos += measureCodepoint(buf[strPos]);
    }
    return utf8ToWChar(buf + strPos);
}

String String::replace(const String& fnd, const String& rplace) const {
    PGE_ASSERT(fnd.byteLength() != 0, "Find string can't be empty");

    const char* fndStr = fnd.cstr();
    const char* rplaceStr = rplace.cstr();
    const char* thisStr = cstr();

    std::vector<int> foundPositions;
    for (int i = 0; i <= byteLength() - fnd.byteLength();) {
        if (memcmp(fndStr, thisStr + i, fnd.byteLength()) == 0) {
            foundPositions.push_back(i);
            i += fnd.byteLength();
        } else {
            i++;
        }
    }
    
    int newSize = byteLength() + foundPositions.size() * (rplace.byteLength() - fnd.byteLength());
    String retVal(newSize);
    retVal._strByteLength = newSize;

    char* retBuf = retVal.cstrNoConst();
    int retPos = 0;
    int thisPos = 0;
    for (int pos : foundPositions) {
        int thisLen = pos - thisPos;
        memcpy(retBuf + retPos, thisStr + thisPos, thisLen);
        retPos += thisLen;
        memcpy(retBuf + retPos, rplaceStr, rplace.byteLength());
        retPos += rplace.byteLength();
        thisPos = pos + fnd.byteLength();
    }
    // Append the rest of the string, including terminating byte.
    memcpy(retBuf + retPos, thisStr + thisPos, byteLength() - thisPos + 1);
   
    // If the string that is being operated on already has had its length calculated, we assume it to be worth it to pre-calculate the new string's length.
    if (_strLength >= 0) {
        retVal._strLength = _strLength + foundPositions.size() * (rplace.length() - fnd.length());
    }
    return retVal;
}

String String::toUpper() const {
    wchar* newBuf = new wchar[byteLength() * sizeof(wchar) + 1];
    wstr(newBuf);
    for (int i = 0; i < byteLength(); i++) {
        newBuf[i] = Char::toUpper(newBuf[i]);
    }
    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::toLower() const {
    wchar* newBuf = new wchar[byteLength() * sizeof(wchar) + 1];
    wstr(newBuf);
    for (int i = 0; i < byteLength(); i++) {
        newBuf[i] = Char::toLower(newBuf[i]);
    }
    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::trim() const {
    if (byteLength() == 0) { return *this; }

    const char* buf = cstr();
    int leadingPos = 0;
    while (buf[leadingPos] == ' ' || buf[leadingPos] == '\t') {
        leadingPos++;
        if (leadingPos>=byteLength()) {
            return *this;
        }
    }

    int trailingPos = byteLength() - 1;
    while (charAt(trailingPos) == ' ' || charAt(trailingPos) == '\t') {
        trailingPos--;
        if (trailingPos<0) {
            return *this;
        }
    }

    int newSize = trailingPos - leadingPos + 1;
    String ret(*this, leadingPos, newSize);
    // If the length has been calculated, good, it remains valid, if not, it just goes more into the negative.
    ret._strLength = _strLength - (byteLength() - newSize);
    return ret;
}

std::vector<String> String::split(const String& needleStr, bool removeEmptyEntries) const {
    std::vector<String> retVal;
    const char* haystack = cstr();
    const char* needle = needleStr.cstr();
    int codepoint;
    int cut = 0;
    for (int i = 0; i <= byteLength() - needleStr.byteLength(); i += codepoint) {
        codepoint = measureCodepoint(haystack[i]);
        if (memcmp(haystack + i, needle, codepoint) == 0) {
            int addSize = i - cut;
            if (!removeEmptyEntries || addSize != 0) {
                retVal.push_back(String(*this, cut, addSize));
            }
            cut = i + needleStr.byteLength();
        }
    }
    // Add the rest of the string to the vector.
    int endAddSize = byteLength() - cut;
    if (endAddSize != 0) {
        retVal.push_back(String(*this, cut, endAddSize));
    }

    return retVal;
}

String String::join(const std::vector<String>& vect, const String& separator) {
    if (vect.size() == 0) {
        return String();
    }

    String retVal = vect[0];
    for (int i = 1; i < (int)vect.size(); i++) {
        retVal = retVal + separator + vect[i];
    }

    return retVal;
}

std::cmatch String::regexMatch(const std::regex& pattern) const {
    const char* s = cstr();
    std::cmatch m;
    std::regex_search(s, m, pattern);

    return m;
}

/* TODO: Improve this if we get MacOS support back. Non-fixed return value size and possibly propagating more metadata.
String String::unHex() const {
    int isUnhexing = 0;

    const char* buf = cstr();

    String ret(64);
    char* retBuf = ret.cstrNoConst();
    int resultSize = 0;

    int tempAscVal = 0;
    int codepoint;
    for (int i=0;i<byteLength();i+=codepoint) {
        codepoint = measureCodepoint(buf[i]);
        if (codepoint == 1) {
            if (isUnhexing == 0) {
                if (buf[i] == '%') {
                    isUnhexing = 1;
                } else {
                    retBuf[resultSize] = buf[i];
                    resultSize++;
                }
            } else {
                int toDec = 0;
                if (buf[i] >= '0' && buf[i] <= '9') {
                    toDec = buf[i] - '0';
                } else if (buf[i] >= 'a' && buf[i] <= 'f') {
                    toDec = buf[i] + 10 - 'a';
                } else if (buf[i] >= 'A' && buf[i] <= 'F') {
                    toDec = buf[i] + 10 - 'A';
                }
                if (isUnhexing == 1) {
                    tempAscVal = toDec << 4;
                    isUnhexing = 2;
                } else if (isUnhexing == 2) {
                    tempAscVal |= toDec;
                    retBuf[resultSize] = tempAscVal;
                    resultSize++;
                    isUnhexing = 0;
                }
            }
        } else {
            for (int j = 0; j < codepoint; j++) {
                retBuf[i + j] = buf[i + j];
            }
        }
    }
    retBuf[resultSize]='\0';
    ret._strByteLength = resultSize;
    return ret;
}*/

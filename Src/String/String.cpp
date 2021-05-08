#include <String/String.h>
#include "Char.h"

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
// Iterator
//

String::Iterator::Iterator() {
    ref = nullptr;
    index = -1;
    charIndex = -1;
}

String::Iterator::Iterator(const String& str) {
    ref = &str;
    index = 0;
    charIndex = 0;
}

String::Iterator::Iterator(const String& str, int byteIndex, int chIndex) {
    ref = &str;
    index = byteIndex;
    charIndex = chIndex;
}

String::Iterator& String::Iterator::operator++() {
    PGE_ASSERT(index < ref->byteLength(), "Can't increment iterator past string end");
    index += measureCodepoint(ref->cstr()[index]);
    charIndex++;
    // We reached the end and get the str length for free.
    if (index == ref->byteLength()) {
        ref->_strLength = charIndex;
    }
    _ch = L'\uFFFF';
    return *this;
}

String::Iterator& String::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

String::Iterator String::Iterator::operator+(int steps) {
    PGE_ASSERT(steps >= 0, "String iterators cannot go backwards");
    String::Iterator ret = *this;
    for (int i = 0; i < steps; i++) {
        ++ret;
    }
    return ret;
}

void String::Iterator::operator+=(int steps) {
    PGE_ASSERT(steps >= 0, "String iterators cannot go backwards");
    for (int i = 0; i < steps; i++) {
        ++*this;
    }
}

wchar String::Iterator::operator*() const {
    genChar();
    return _ch;
}

const wchar* String::Iterator::operator->() const {
    genChar();
    return &_ch;
}

bool String::Iterator::operator==(const Iterator& other) const {
    return ref == other.ref && index == other.index;
}

bool String::Iterator::operator!=(const Iterator& other) const {
    return ref != other.ref || index != other.index;
}

int String::Iterator::getPosition() const {
    return charIndex == -1 ? ref->length() : charIndex;
}

void String::Iterator::genChar() const {
    if (_ch == L'\uFFFF') {
        _ch = utf8ToWChar(ref->cstr() + index);
    }
}

String::Iterator String::begin() const {
    return Iterator(*this);
}

String::Iterator String::end() const {
    // We need byteLength for functionality, but length is optional.
    return Iterator(*this, byteLength(), _strLength);
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

void String::operator=(const String& other) {
    if (cstr()[0] == '\0' && other.cstr()[0] == '\0') { return; }
    reallocate(other.byteLength());
    memcpy(cstrNoConst(), other.cstr(), other.byteLength() + 1);
    _strByteLength = other._strByteLength;
    _strLength = other._strLength;
    _hashCodeEvaluted = other._hashCodeEvaluted;
    _hashCode = other._hashCode;
}

void String::operator+=(const String& other) {
    int oldByteSize = byteLength();
    int newSize = oldByteSize + other.byteLength();
    reallocate(newSize, true);
    char* buf = cstrNoConst();
    memcpy(buf + oldByteSize, other.cstr(), other.byteLength() + 1);
    _strByteLength = newSize;
    if (_strLength >= 0 && other._strLength >= 0) {
        _strLength += other.length();
    }
}

void String::operator+=(wchar ch) {
    int aLen = byteLength();
    reallocate(aLen + 4, true);
    char* buf = cstrNoConst();
    memcpy(buf, cstr(), aLen);
    int actualSize = aLen + convertWCharToUtf8(ch, buf + aLen);
    buf[actualSize] = '\0';
    _strByteLength = actualSize;
    if (_strLength >= 0) {
        _strLength++;
    }
}

const String PGE::operator+(const String& a, const String& b) {
    int aLen = a.byteLength();
    int bLen = b.byteLength();
    String ret = String(aLen + bLen);
    char* buf = ret.cstrNoConst();
    memcpy(buf, a.cstr(), aLen);
    memcpy(buf + aLen, b.cstr(), bLen + 1);
    ret._strByteLength = aLen + bLen;
    if (a._strLength >= 0 && b._strLength >= 0) {
        ret._strLength = a.length() + b.length();
    }
    return ret;
}

const String PGE::operator+(const char* a, const String& b) {
    int aLen = strlen(a);
    int bLen = b.byteLength();
    String ret = String(aLen + bLen);
    char* buf = ret.cstrNoConst();
    memcpy(buf, a, aLen);
    memcpy(buf + aLen, b.cstr(), bLen + 1);
    ret._strByteLength = aLen + bLen;
    return ret;
}

const String PGE::operator+(const String& a, wchar b) {
    int aLen = a.byteLength();
    String ret = String(aLen + 4);
    char* buf = ret.cstrNoConst();
    memcpy(buf, a.cstr(), aLen);
    int actualSize = aLen + convertWCharToUtf8(b, buf + aLen);
    buf[actualSize] = '\0';
    ret._strByteLength = actualSize;
    if (a._strLength >= 0) {
        ret._strLength = a.length() + 1;
    }
    return ret;
}

bool PGE::operator==(const String& a, const String& b) {
    return a.equals(b);
}

bool PGE::operator!=(const String& a, const String& b) {
    return !a.equals(b);
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

void String::reallocate(int size, bool copyOldData) {
    // Invalidating metadata.
    _hashCodeEvaluted = false;
    _strLength = -1;
    _strByteLength = -1;

    // Accounting for the terminating byte.
    size++;
    if (size <= shortStrCapacity || size <= cCapacity) { return; }
    int targetCapacity = 1;
    while (targetCapacity < size) { targetCapacity <<= 1; }

    char* newData = new char[targetCapacity];

    if (copyOldData) {
        memcpy(newData, cstr(), byteLength());
    }

    if (cCapacity > 0) {
        delete[] data.longStr;
    }
    cCapacity = targetCapacity;
    data.longStr = newData;
}

const char* String::cstr() const {
    return cCapacity > 0 ? data.longStr : data.shortStr;
}

char* String::cstrNoConst() {
    return cCapacity > 0 ? data.longStr : data.shortStr;
}

void String::wstr(wchar* buffer) const {
    // Convert all the codepoints to wchars.
    for (Iterator it = begin(); it != end(); ++it) {
        buffer[it.getPosition()] = *it;
    }
    buffer[length()] = '\0';
}

int String::toInt(bool& success) const {
    try {
        success = true;
        return std::stoi(cstr());
    } catch (const std::exception&) {
        success = false;
        return 0;
    }
}

float String::toFloat(bool& success) const {
    try {
        success = true;
        return std::stof(cstr());
    } catch (const std::exception&) {
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

String::Iterator String::findFirst(const String& fnd, int from) const {
    return findFirst(fnd, begin() + from);
}

String::Iterator String::findFirst(const String& fnd, const Iterator& from) const {
    PGE_ASSERT(!fnd.isEmpty(), "Find string can't be empty");
    for (auto it = from; it != end(); ++it) {
        if (memcmp(fnd.cstr(), cstr() + it.index, fnd.byteLength()) == 0) { return it; }
    }
    return end();
}

String::Iterator String::findLast(const String& fnd, int from) const {
    return findLast(fnd, begin() + from);
}

String::Iterator String::findLast(const String& fnd, const Iterator& from) const {
    PGE_ASSERT(!fnd.isEmpty(), "Find string can't be empty");
    String::Iterator found = end();
    for (auto it = from; it != end(); ++it) {
        if (memcmp(fnd.cstr(), cstr() + it.index, fnd.byteLength()) == 0) { found = it; }
    }
    return found;
}

String String::substr(int start) const {
    return substr(begin() + start);
}

String String::substr(int start, int cnt) const {
    Iterator from = begin() + start;
    return substr(from, from + cnt);
}

String String::substr(const Iterator& start) const {
    return substr(start, end());
}

String String::substr(const Iterator& start, const Iterator& to) const {
    PGE_ASSERT(start.index <= to.index, "Start iterator can't come after to iterator (start: " + fromInt(start.index) + "; to: " + fromInt(to.index) + "; str: " + *this + ")");
    PGE_ASSERT(to.index <= end().index, "To iterator can't come after end iterator (to: " + fromInt(to.index) + "; end: " + fromInt(end().index) + "; str: " + *this + ")");

    int newSize = to.index - start.index;
    String retVal(newSize);
    retVal._strByteLength = newSize;
    if (to.charIndex >= 0) {
        retVal._strLength = to.charIndex - start.charIndex;
    }
    char* retBuf = retVal.cstrNoConst();
    memcpy(retBuf, cstr() + start.index, newSize);
    retBuf[newSize] = '\0';
    return retVal;
}

String::Iterator String::charAt(int pos) const {
    Iterator it;
    for (it = begin(); it != end() && it.charIndex != pos; ++it);
    return it;
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
    while (*charAt(trailingPos) == ' ' || *charAt(trailingPos) == '\t') {
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

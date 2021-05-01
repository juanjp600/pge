#include <Misc/String.h>

#include <cstring>
#include <cwchar>
#include <cstdlib>
#include <cwctype>
#include <iostream>
#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#endif

using namespace PGE;

String::~String() {
    if (cCapacity > 0) {
        delete[] data.longStr;
    }
}

String::String() {
    reset();
}

String::String(const String& a) {
    reset();
    *this = a;
}

String::String(const char* cstri) {
    int len = (int)strlen(cstri);
    reallocate(len);
    memcpy(cstrNoConst(), cstri, (len+1)*sizeof(char));
    recalculateHashAndLength();
}

String::String(const std::string& cppstr) {
    int len = (int)cppstr.size();
    reallocate(len);
    memcpy(cstrNoConst(), cppstr.c_str(), (len+1)*sizeof(char));
    recalculateHashAndLength();
}

String::String(const wchar* wstri) {
    wCharToUtf8Str(wstri);
    recalculateHashAndLength();
}

String::String(const std::wstring& cppwstr) {
    wCharToUtf8Str(cppwstr.c_str());
    recalculateHashAndLength();
}

#if defined(__APPLE__) && defined(__OBJC__)
String::String(const NSString* nsstr) {
    const char* cPath = [nsstr cStringUsingEncoding: NSUTF8StringEncoding];
    int len = (int)strlen(cPath);
    reallocate(len);
    memcpy(cstrNoConst(), cPath, (len+1)*sizeof(char));
    recalculateHashAndLength();
}
#endif

String::String(const String& a, const String& b) {
    int len = a.byteLength() + b.byteLength();
    reallocate(len);
    char* buf = cstrNoConst();
    memcpy(buf, a.cstr(), a.byteLength());
    memcpy(buf + a.byteLength(), b.cstr(), b.byteLength());
    buf[len] = '\0';
    recalculateHashAndLength();
}

String::String(char c) {
    reallocate(1);
    char* buf = cstrNoConst();
    buf[0] = c; buf[1] = '\0';
    recalculateHashAndLength();
}

String::String(wchar w) {
    wchar* tempBuf = new wchar[2];
    tempBuf[0] = w; tempBuf[1] = L'\0';
    wCharToUtf8Str(tempBuf);
    delete[] tempBuf;
    recalculateHashAndLength();
}

String::String(int size) {
    reallocate(size);
}

template <class T>
String String::format(T t, const String& format) {
    int size = snprintf(nullptr, 0, format.cstr(), t) + 1;
    if (size < 0) {
        throw std::runtime_error(("Invalid format: " + format).cstr());
    }
    String ret(size);
    snprintf(ret.cstrNoConst(), size, format.cstr(), t);
    ret.recalculateHashAndLength();
    return ret;
}

// TODO: Possibly remove
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
    String ret(32);
    snprintf(ret.cstrNoConst(), 31, "%d", i);
    ret.recalculateHashAndLength();
    return ret;
}

String String::fromFloat(float f) {
    String ret(32);
    snprintf(ret.cstrNoConst(), 31, "%f", f);
    ret.recalculateHashAndLength();
    return ret;
}

String& String::operator=(const String& other) {
    if (&other == this || other == *this) { return *this; }
    reallocate(other.byteLength());
    memcpy(cstrNoConst(),other.cstr(),(other.byteLength()+1)*sizeof(char));
    strByteLength = other.byteLength();
    strLength = other.length();
    hashCode = other.getHashCode();
    return *this;
}

String& String::operator+=(const String& other) {
    if (&other == this) return *this;
    *this=String(*this, other);
    return *this;
}

bool PGE::operator==(const String& a, const String& b) {
    if (a.byteLength() != b.byteLength()) { return false; }
    return a.getHashCode() == b.getHashCode();
}

bool PGE::operator!=(const String& a, const String& b) {
    if (a.byteLength() != b.byteLength()) { return true; }
    return a.getHashCode() != b.getHashCode();
}

const String PGE::operator+(const String& a, const String& b) {
    return String(a, b);
}

const String PGE::operator+(const char* a, const String& b) {
    return String(String(a), b);
}

bool PGE::operator<(const String& a, const String& b) {
    return strcmp(a.cstr(), b.cstr()) < 0;
}

bool PGE::operator>(const String& a, const String& b) {
    return strcmp(a.cstr(), b.cstr()) > 0;
}

std::ostream& PGE::operator<<(std::ostream& os, const String& s) {
    return os.write(s.cstr(),s.byteLength());
}

long long String::getHashCode() const {
    return hashCode;
}

bool String::equals(const String& other) const {
    if (other.byteLength()!=byteLength()) { return false; }
    return other.getHashCode()==getHashCode();
}

bool String::equalsIgnoreCase(const String& other) const {
    // TODO: Other implementation for this as well?
    if (byteLength() != other.byteLength()) { return false; }
    if (other.getHashCode() == getHashCode()) { return true; }

    wchar* w1 = new wchar[other.byteLength() * sizeof(wchar) + 1];
    wchar* w2 = new wchar[byteLength() * sizeof(wchar) + 1];

    other.wstr(w1);
    wstr(w2);

    bool equals = true;
    for (int i = 0; i<strByteLength; i++) {
        if (w1[i] != w2[i]) {
            equals = false;
            break;
        }
    }
    delete[] w1;
    delete[] w2;
    return equals;
}

bool String::isEmpty() const {
    return strByteLength == 0;
}

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

static int convertWCharToUtf8(wchar chr, char* result) {
    //fits in standard ASCII, just return the char as-is
    if ((chr & 0x7f) == chr) {
        if (result != nullptr) { result[0] = (char)chr; }
        return 1;
    }

    int len = 1;

    //determine most of the bytes after the first one
    while ((chr & (~0x3f)) != 0x00) {
        if (result != nullptr) { result[len - 1] = 0x80 | (chr & 0x3f); }
        chr >>= 6;
        len++;
    }

    //determine the remaining byte(s): if the number of free bits in
    //the first byte isn't enough to fit the remaining bits,
    //add another byte
    char firstByte = 0x00;
    for (int i = 0; i < len; i++) {
        firstByte |= (0x1 << (7-i));
    }

    if (((firstByte | (0x1 << (7-len))) & chr) == 0x00) {
        //it fits!
        firstByte = firstByte | chr;
        if (result != nullptr) { result[len - 1] = firstByte; }
    } else {
        //it doesn't fit: add another byte
        if (result != nullptr) { result[len - 1] = 0x80 | (chr & 0x3f); }
        chr >>= 6;
        firstByte = (firstByte | (0x1 << (7 - len))) | chr;
        len++;
        if (result != nullptr) { result[len - 1] = firstByte; }
    }

    if (result != nullptr) {
        //flip the result
        for (int i = 0; i < len/2; i++) {
            char b = result[i];
            result[i] = result[len - 1 - i];
            result[len - 1 - i] = b;
        }
    }

    return len;
}

void String::recalculateHashAndLength() {
    hashCode = 5381;
    const char* buf = cstrNoConst();
    strByteLength = strlen(buf);
    strLength = 0;
    int capacity = cCapacity > 0 ? cCapacity : shortStrCapacity;
    for (int i = 0; i < capacity; i += measureCodepoint(buf[i])) {
        if (buf[i] == '\0') {
            break;
        }
        strLength++;
        hashCode = ((hashCode << 5) + hashCode) + buf[i];
    }
}

void String::reallocate(int byteLength) {
    if ((byteLength+1) < shortStrCapacity) { return; }
    int targetCapacity = 1;
    while (targetCapacity < (byteLength+1)) { targetCapacity <<= 1; }
    if (targetCapacity <= cCapacity) { return; }

    if (cCapacity < targetCapacity) {
        if (cCapacity > 0) {
            delete[] data.longStr;
        }
        cCapacity = targetCapacity;
        data.longStr = new char[targetCapacity];
    }
}

void String::reset() {
    reallocate(0);
    data.shortStr[0]='\0';
    recalculateHashAndLength();
}

void String::wCharToUtf8Str(const wchar* wbuffer) {
    //determine the capacity of the cbuffer by measuring the number of bytes required for each codepoint
    strByteLength = 0;
    for (int i = 0; wbuffer[i] != L'\0'; i++) {
        strByteLength += convertWCharToUtf8(wbuffer[i], nullptr);
    }
    reallocate(strByteLength);

    //convert all the wchars to codepoints
    char* buf = cstrNoConst();
    int cIndex = 0;
    int i = 0;
    for (int i = 0; wbuffer[i] != L'\0'; i++) {
        int increment = convertWCharToUtf8(wbuffer[i], &buf[cIndex]);
        cIndex += increment;
    }
    buf[cIndex] = '\0';
}

wchar String::utf8ToWChar(const char* cbuffer) {
    int codepointLen = measureCodepoint(cbuffer[0]);
    if (codepointLen == 1) {
        return cbuffer[0];
    } else {
        //decode first byte by skipping all bits that indicate the length of the codepoint
        wchar newChar = cbuffer[0] & (0x7f >> codepointLen);
        for (int j = 1; j < codepointLen; j++) {
            //decode all of the following bytes, fixed 6 bits per byte
            newChar = (newChar << 6) | (cbuffer[j] & 0x3f);
        }
        return newChar;
    }
}

int String::length() const {
    return strLength;
}

int String::byteLength() const {
    return strByteLength;
}

int String::findFirst(const String& fnd, int from) const {
    if (fnd.byteLength() == 0) { return -1; }
    if (from < 0) { from = 0; }
    int charPos = 0;
    for (int i = 0; i <= byteLength()-fnd.byteLength(); i += measureCodepoint(cstr()[i])) {
        if (charPos >= from) {
            if (memcmp(fnd.cstr(), cstr() + i, fnd.byteLength()) == 0) { return charPos; }
        }
        charPos++;
    }
    return -1;
}

int String::findLast(const String& fnd, int from) const {
    if (fnd.byteLength() == 0) { return -1; }
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

const char* String::cstr() const {
    return cCapacity>0 ? data.longStr : data.shortStr;
}

char* String::cstrNoConst() {
    return cCapacity>0 ? data.longStr : data.shortStr;
}

void String::wstr(wchar* buffer) const {
    //convert all the codepoints to wchars
    const char* buf = cstr();
    int wIndex = 0;
    for (int i = 0; i < strByteLength;) {
        int codepointLen = measureCodepoint(buf[i]);

        buffer[wIndex] = utf8ToWChar(buf+i);

        i += codepointLen;
        wIndex++;
    }
    buffer[strByteLength] = '\0';
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

String String::substr(int start, int cnt) const {
    if ((cnt<0) || (cnt+start>=byteLength())) {
        cnt = byteLength()-start;
    }

    const char* buf = cstr();

    int startPos = 0;
    for (int i = 0; i < start; i++) {
        startPos += measureCodepoint(buf[startPos]);
    }

    int actualSize = 0;
    for (int i = 0; i < cnt; i++) {
        actualSize += measureCodepoint(buf[startPos + actualSize]);
    }

    String retVal(actualSize + 1);
    char* retBuf = retVal.cstrNoConst();
    retBuf[actualSize] = '\0';
    memcpy(retBuf, buf + startPos, actualSize);
    retVal.recalculateHashAndLength();
    return retVal;
}

wchar String::charAt(int pos) const {
    const char* buf = cstr();
    int strPos = 0;
    for (int i = 0; i < pos; i++) {
        strPos += measureCodepoint(buf[strPos]);
    }
    return utf8ToWChar(buf+strPos);
}

String String::replace(const String& fnd, const String& rplace) const {
    if (fnd.byteLength()==0) { return *this; }

    const char* fndStr = fnd.cstr();
    const char* rplaceStr = rplace.cstr();
    const char* thisStr = cstr();

    int newSize = strByteLength;
    for (int i=0;i<strByteLength-fnd.byteLength()+1;) {
        if (memcmp(fndStr, thisStr+i,fnd.byteLength())==0) {
            newSize+=rplace.byteLength()-fnd.byteLength();
            i+=fnd.byteLength();
        } else {
            i++;
        }
    }

    String retVal(newSize);
    char* retBuf = retVal.cstrNoConst();
    int i=0; int j=0;
    while (i<strByteLength) {
        bool found = i<strByteLength-fnd.byteLength()+1;
        if (found) {
            found = memcmp(fndStr,thisStr+i,fnd.byteLength())==0;
        }
        if (found) {
            memcpy(retBuf+j,rplaceStr,rplace.byteLength());
            i+=fnd.byteLength(); j+=rplace.byteLength();
        } else {
            retBuf[j]=thisStr[i];
            i++; j++;
        }
    }
    retBuf[j]=L'\0';

    retVal.recalculateHashAndLength();
    return retVal;
}

String String::toUpper() const {
    wchar* newBuf = new wchar[strByteLength * sizeof(wchar) + 1];
    wstr(newBuf);
    for (int i = 0; i<strByteLength; i++) {
        newBuf[i] = towupper(newBuf[i]);
    }
    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::toLower() const {
    wchar* newBuf = new wchar[strByteLength * sizeof(wchar) + 1];
    wstr(newBuf);
    for (int i = 0; i<strByteLength; i++) {
        newBuf[i] = towlower(newBuf[i]);
    }
    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::trim() const {
    if (strByteLength==0) { return ""; }

    const char* buf = cstr();
    int leadingPos = 0;
    while (buf[leadingPos] == ' ' || buf[leadingPos] == '\t') {
        leadingPos++;
        if (leadingPos>=strByteLength) {
            return *this;
        }
    }

    int trailingPos = byteLength()-1;
    while (charAt(trailingPos) == ' ' || charAt(trailingPos) == '\t') {
        trailingPos--;
        if (trailingPos<0) {
            return *this;
        }
    }

    int newSize = trailingPos - leadingPos + 1;
    String ret(newSize + 1);
    char* retBuf = ret.cstrNoConst();
    memcpy(retBuf, buf + leadingPos, newSize);
    retBuf[newSize] = '\0';
    ret.recalculateHashAndLength();
    return ret;
}

std::vector<String> String::split(const String& needle, bool removeEmptyEntries) const {
    std::vector<String> retVal;
    String haystack = String(*this);
    while (haystack.findFirst(needle) > -1) {
        String adder = haystack.substr(0, haystack.findFirst(needle));
        retVal.push_back(adder);
        haystack = haystack.substr(adder.byteLength()+needle.byteLength());
    }
    // Add the rest of the string to the vector.
    retVal.push_back(haystack);

    if (removeEmptyEntries) {
        for (int i = 0; i < (int)retVal.size(); i++) {
            if (retVal[i].isEmpty()) {
                retVal.erase(retVal.begin() + i);
                i--;
            }
        }
    }

    return retVal;
}

String String::join(const std::vector<String>& vect, const String& separator) {
    if (vect.size() <= 0) {
        return String("");
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
    ret.recalculateHashAndLength();
    return ret;
}

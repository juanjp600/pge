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
    delete[] cbuffer;
}

String::String() {
    cbuffer = new char[24];
    cCapacity = 24;
    cbuffer[0]='\0';
    syncBuffers();
}

String::String(const String& a) {
    int len = a.byteSize() + 1;
    cbuffer = new char[len];
    cCapacity = len;
    memcpy(cbuffer, a.cstr(), len);
    syncBuffers();
}

String::String(const char* cstri) {
    int len = (int)strlen(cstri);
    cbuffer = new char[len+1];
    cCapacity = len+1;
    memcpy(cbuffer, cstri, (len+1)*sizeof(char));
    syncBuffers();
}

String::String(const std::string& cppstr) {
    int len = (int)cppstr.size();
    cbuffer = new char[len+1];
    cCapacity = len+1;
    memcpy(cbuffer, cppstr.c_str(), (len+1)*sizeof(char));
    syncBuffers();
}

String::String(const wchar* wstri) {
    wCharToUtf8(wstri);
    syncBuffers();
}

String::String(const std::wstring& cppwstr) {
    wCharToUtf8(cppwstr.c_str());
    syncBuffers();
}

#if defined(__APPLE__) && defined(__OBJC__)
String::String(const NSString* nsstr) {
    const char* cPath = [nsstr cStringUsingEncoding: NSUTF8StringEncoding];
    int len = (int)strlen(cPath);
    cbuffer = new char[len+1];
    cCapacity = len+1;
    memcpy(cbuffer, cPath, (len+1)*sizeof(char));
    syncBuffers();
}
#endif

String::String(const String& a, const String& b) {
    int len = a.byteSize() + b.byteSize() + 1;
    cbuffer = new char[len];
    cCapacity = len;
    memcpy(cbuffer, a.cstr(), a.byteSize());
    memcpy(cbuffer + a.byteSize(), b.cstr(), b.byteSize());
    cbuffer[len - 1] = '\0';
    syncBuffers();
}

String::String(char c) {
    cbuffer = new char[2];
    cCapacity = 2;
    cbuffer[0] = c; cbuffer[1] = '\0';
    syncBuffers();
}

String::String(wchar w) {
    wchar* tempBuf = new wchar[2];
    tempBuf[0] = w; tempBuf[1] = L'\0';
    wCharToUtf8(tempBuf);
    delete[] tempBuf;
    syncBuffers();
}

String::String(int size) {
    cbuffer = new char[size];
    cCapacity = size;
}

template <class T>
String String::format(T t, const String& format) {
    int size = snprintf(nullptr, 0, format.cstr(), t) + 1;
    if (size < 0) {
        throw new std::runtime_error(("Invalid format: " + format).cstr());
    }
    String ret(size);
    snprintf(ret.cbuffer, size, format.cstr(), t);
    ret.syncBuffers();
    return ret;
}

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
    snprintf(ret.cbuffer, 32, "%i", i);
    ret.syncBuffers();
    return ret;
}

String String::fromFloat(float f) {
    String ret(32);
    snprintf(ret.cbuffer, 32, "%f", f);
    ret.syncBuffers();
    return ret;
}

String& String::operator=(const String& other) {
    if (&other == this) return *this;
    if (cbuffer!=nullptr) { delete[] cbuffer; }
    cbuffer = new char[other.cCapacity];
    memcpy(cbuffer,other.cstr(),(other.size()+1)*sizeof(char));
    cCapacity = other.cCapacity;
    strSize = other.size();
    hashCode = other.getHashCode();
    return *this;
}

String& String::operator+=(const String& other) {
    if (&other == this) return *this;
    *this=String(*this, other);
    return *this;
}

bool PGE::operator==(const String& a, const String& b) {
    if (a.size() != b.size()) { return false; }
    return a.getHashCode() == b.getHashCode();
}

bool PGE::operator!=(const String& a, const String& b) {
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
    return os.write(s.cstr(),s.size());
}

long long String::getHashCode() const {
    return hashCode;
}

bool String::equals(const String& other) const {
    if (other.size()!=size()) { return false; }
    return other.getHashCode()==getHashCode();
}

bool String::equalsIgnoreCase(const String& other) const {
    // TODO: Other implementation for this as well?
    if (size() != other.size()) { return false; }
    if (other.getHashCode() == getHashCode()) { return true; }

    wchar* w1 = new wchar[other.size() * sizeof(wchar) + 1];
    wchar* w2 = new wchar[size() * sizeof(wchar) + 1];

    other.wstr(w1);
    wstr(w2);

    bool equals = true;
    for (int i = 0; i<strSize; i++) {
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
    return strSize == 0;
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

void String::syncBuffers() {
    strSize = 0;
    hashCode = 5381;
    for (int i = 0; i < cCapacity; i += measureCodepoint(cbuffer[i])) {
        if (cbuffer[i] == '\0') {
            break;
        }
        strSize++;
        hashCode = ((hashCode << 5) + hashCode) + cbuffer[i];
    }
}

void String::wCharToUtf8(const wchar* wbuffer) {
    if (cbuffer != nullptr) { delete[] cbuffer; }

    //determine the capacity of the cbuffer by measuring the number of bytes required for each codepoint
    strSize = 0;
    cCapacity = 1;
    for (int i = 0; wbuffer[i] != L'\0'; i++) {
        strSize++;
        cCapacity += convertWCharToUtf8(wbuffer[i], nullptr);
    }

    //convert all the wchars to codepoints
    cbuffer = new char[cCapacity];
    char tempBuf[8]; // TODO: Remove this?
    int cIndex = 0;
    int i = 0;
    for (int i = 0; wbuffer[i] != L'\0'; i++) {
        int increment = convertWCharToUtf8(wbuffer[i], tempBuf);
        for (int j = 0; j < increment; j++) {
            cbuffer[cIndex] = tempBuf[j];
            cIndex++;
        }
    }
    cbuffer[cIndex] = '\0';
}

int String::size() const {
    return strSize;
}

int String::byteSize() const {
    return (int)strlen(cstr());
}

int String::findFirst(const String& fnd, int from) const {
    if (fnd.size() == 0) { return -1; }
    if (from < 0) { from = 0; }
    int charPos = 0;
    for (int i = 0; i <= size()-fnd.size(); i += measureCodepoint(cbuffer[i])) {
        if (charPos >= from) {
            if (memcmp(fnd.cstr(), cstr() + i, fnd.byteSize()) == 0) { return charPos; }
        }
        charPos++;
    }
    return -1;
}

int String::findLast(const String& fnd, int from) const {
    if (fnd.size() == 0) { return -1; }
    if (from < 0) { from = 0; }
    int charPos = 0;
    int foundPos = -1;
    for (int i = 0; i <= size() - fnd.size(); i += measureCodepoint(cbuffer[i])) {
        if (charPos >= from) {
            if (memcmp(fnd.cstr(), cstr() + i, fnd.size() * sizeof(wchar)) == 0) { foundPos = charPos; }
        }
        charPos++;
    }
    return foundPos;
}

const char* String::cstr() const {
    return cbuffer;
}

void String::wstr(wchar* buffer) const {
    //convert all the codepoints to wchars
    int wIndex = 0;
    for (int i = 0; i < cCapacity;) {
        if (cbuffer[i] == '\0') {
            buffer[wIndex] = L'\0';
            break;
        } else {
            int codepointLen = measureCodepoint(cbuffer[i]);

            if (codepointLen == 1) {
                buffer[wIndex] = cbuffer[i];
            } else {
                //decode first byte by skipping all bits that indicate the length of the codepoint
                wchar newChar = cbuffer[i] & (0x7f >> codepointLen);
                for (int j = 1; j < codepointLen; j++) {
                    //decode all of the following bytes, fixed 6 bits per byte
                    newChar = (newChar << 6) | (cbuffer[i + j] & 0x3f);
                }
                buffer[wIndex] = newChar;
            }

            i += codepointLen;
            wIndex++;
        }
    }
}

int String::toInt(bool& success) const {
    try {
        success = true;
        return std::stoi(cbuffer);
    } catch (std::exception) {
        success = false;
        return 0;
    }
}

float String::toFloat(bool& success) const {
    try {
        success = true;
        return std::stof(cbuffer);
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
    if ((cnt<0) || (cnt+start>=size())) {
        cnt = size()-start;
    }

    int startPos = 0;
    for (int i = 0; i < start; i++) {
        startPos += measureCodepoint(cbuffer[startPos]);
    }

    int actualSize = 0;
    for (int i = 0; i < cnt; i++) {
        actualSize += measureCodepoint(cbuffer[startPos + actualSize]);
    }

    String retVal(actualSize + 1);
    retVal.cbuffer[actualSize] = '\0';
    memcpy(retVal.cbuffer, cbuffer + startPos, actualSize);
    retVal.syncBuffers();
    return retVal;
}

char String::charAt(int pos) const {
    // TODO: Look at this, returns garbage for non-asci.
    int strPos = 0;
    for (int i = 0; i < pos; i++) {
        strPos += measureCodepoint(cbuffer[strPos]);
    }
    return cbuffer[strPos];
}

String String::replace(const String& fnd, const String& rplace) const {
    if (fnd.size()==0) { return *this; }

    // TODO: Avoid wstr.

    wchar* fndStr = new wchar[fnd.size() + 1];
    wchar* rplaceStr = new wchar[rplace.size() + 1];
    wchar* thisStr = new wchar[size() + 1];

    fnd.wstr(fndStr);
    rplace.wstr(rplaceStr);
    wstr(thisStr);

    int newSize = strSize;
    for (int i=0;i<strSize-fnd.size()+1;) {
        if (memcmp(fndStr, thisStr+i,fnd.size()*sizeof(wchar))==0) {
            newSize+=rplace.size()-fnd.size();
            i+=fnd.size();
        } else {
            i++;
        }
    }

    wchar* newBuf = new wchar[newSize+1];
    int i=0; int j=0;
    while (i<strSize) {
        bool found = i<strSize-fnd.size()+1;
        if (found) {
            found = memcmp(fndStr,thisStr+i,fnd.size()*sizeof(wchar))==0;
        }
        if (found) {
            memcpy(newBuf+j,rplaceStr,rplace.size()*sizeof(wchar));
            i+=fnd.size(); j+=rplace.size();
        } else {
            newBuf[j]=thisStr[i];
            i++; j++;
        }
    }
    newBuf[j]=L'\0';

    String retVal(newBuf);
    delete[] newBuf;
    delete[] fndStr;
    delete[] rplaceStr;
    delete[] thisStr;
    return retVal;
}

String String::toUpper() const {
    wchar* newBuf = new wchar[strSize * sizeof(wchar) + 1];
    wstr(newBuf);
    for (int i = 0; i<strSize; i++) {
        newBuf[i] = towupper(newBuf[i]);
    }
    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::toLower() const {
    // TODO: Probably more consistent and effective with a custom implementation.
    wchar* newBuf = new wchar[strSize * sizeof(wchar) + 1];
    wstr(newBuf);
    for (int i = 0; i<strSize; i++) {
        newBuf[i] = towlower(newBuf[i]);
    }
    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::trim() const {
    if (strSize==0) { return ""; }

    int leadingPos = 0;
    while (cbuffer[leadingPos] == ' ' || cbuffer[leadingPos] == '\t') {
        leadingPos++;
        if (leadingPos>=strSize) {
            return *this;
        }
    }

    int trailingPos = leadingPos;
    int checkPos = trailingPos;
    while (cbuffer[checkPos] != '\0') {
        int codepoint;
        while (codepoint = measureCodepoint(cbuffer[checkPos]) != 1) {
            checkPos += codepoint;
        }
        trailingPos = checkPos;
        while (cbuffer[checkPos] == ' ' || cbuffer[checkPos] == '\t') {
            checkPos++;
        }
        checkPos++;
    }

    int newSize = trailingPos - leadingPos;
    String ret(newSize + 1);
    memcpy(ret.cbuffer, cbuffer + leadingPos, newSize);
    ret.cbuffer[newSize] = '\0';
    ret.syncBuffers();
    return ret;
}

std::vector<String> String::split(const String& needle, bool removeEmptyEntries) const {
    std::vector<String> retVal;
    String haystack = String(*this);
    while (haystack.findFirst(needle) > -1) {
        String adder = haystack.substr(0, haystack.findFirst(needle));
        retVal.push_back(adder);
        haystack = haystack.substr(adder.size()+needle.size());
    }
    // Add the rest of the string to the vector.
    retVal.push_back(haystack);

    if (removeEmptyEntries) {
        for (int i = 0; i < retVal.size(); i++) {
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
    for (int i = 1; i < vect.size(); i++) {
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

    String ret(64);
    int resultSize = 0;

    int tempAscVal = 0;
    int codepoint;
    for (int i=0;i<size();i+=codepoint) {
        codepoint = measureCodepoint(cbuffer[i]);
        if (codepoint == 1) {
            if (isUnhexing == 0) {
                if (cbuffer[i] == '%') {
                    isUnhexing = 1;
                } else {
                    ret.cbuffer[resultSize] = cbuffer[i];
                    resultSize++;
                }
            } else {
                int toDec = 0;
                if (cbuffer[i] >= '0' && cbuffer[i] <= '9') {
                    toDec = cbuffer[i] - '0';
                } else if (cbuffer[i] >= 'a' && cbuffer[i] <= 'f') {
                    toDec = cbuffer[i] + 10 - 'a';
                } else if (cbuffer[i] >= 'A' && cbuffer[i] <= 'F') {
                    toDec = cbuffer[i] + 10 - 'A';
                }
                if (isUnhexing == 1) {
                    tempAscVal = toDec << 4;
                    isUnhexing = 2;
                } else if (isUnhexing == 2) {
                    tempAscVal |= toDec;
                    ret.cbuffer[resultSize] = tempAscVal;
                    resultSize++;
                    isUnhexing = 0;
                }
            }
        } else {
            for (int j = 0; j < codepoint; j++) {
                ret.cbuffer[i + j] = cbuffer[i + j];
            }
        }
    }
    ret.cbuffer[resultSize]='\0';
    ret.syncBuffers();
    return ret;
}

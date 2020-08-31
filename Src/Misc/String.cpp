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
    if (cbuffer!=nullptr) { delete[] cbuffer; }
    if (wbuffer!=nullptr) { delete[] wbuffer; }
}

String::String() {
    cbuffer = new char[24];
    wbuffer = nullptr;
    cCapacity = 24;
    cbuffer[0]='\0';
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(const String& a) {
    int len = a.size();
    wbuffer = new wchar[len+1];
    cbuffer = nullptr;
    wCapacity = len+1;
    memcpy(wbuffer,a.wstr(),(a.size()+1)*sizeof(wchar));
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

String::String(const char* cstri) {
    int len = (int)strlen(cstri);
    cbuffer = new char[len+1];
    wbuffer = nullptr;
    cCapacity = len+1;
    memcpy(cbuffer, cstri, (len+1)*sizeof(char));
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(const std::string& cppstr) {
    int len = (int)cppstr.size();
    cbuffer = new char[len+1];
    wbuffer = nullptr;
    cCapacity = len+1;
    memcpy(cbuffer, cppstr.c_str(), (len+1)*sizeof(char));
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(const wchar* wstri) {
    int len = (int)wcslen(wstri);
    wbuffer = new wchar[len+1];
    cbuffer = nullptr;
    wCapacity = len+1;
    memcpy(wbuffer, wstri, (len+1)*sizeof(wchar));
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

String::String(const std::wstring& cppwstr) {
    int len = (int)cppwstr.size();
    wbuffer = new wchar[len+1];
    cbuffer = nullptr;
    wCapacity = len+1;
    memcpy(wbuffer, cppwstr.c_str(), (len+1)*sizeof(wchar));
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

#if defined(__APPLE__) && defined(__OBJC__)
String::String(const NSString* nsstr) {
    const char* cPath = [nsstr cStringUsingEncoding: NSUTF8StringEncoding];
    int len = (int)strlen(cPath);
    cbuffer = new char[len+1];
    wbuffer = nullptr;
    cCapacity = len+1;
    memcpy(cbuffer, cPath, (len+1)*sizeof(char));
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}
#endif

String::String(const String& a, const String& b) {
    int len = a.size()+b.size();
    wbuffer = new wchar[len+1];
    cbuffer = nullptr;
    wCapacity = len+1;
    memcpy(wbuffer, a.wstr(), a.size() * sizeof(wchar));
    memcpy(wbuffer+a.size(), b.wstr(), (b.size()+1) * sizeof(wchar));
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

String::String(char c) {
    cbuffer = new char[2];
    wbuffer = nullptr;
    cCapacity = 2;
    cbuffer[0] = c; cbuffer[1]='\0';
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(wchar w) {
    wbuffer = new wchar[2];
    cbuffer = nullptr;
    wCapacity = 2;
    wbuffer[0] = w; wbuffer[1]=L'\0';
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

String::String(int32_t i) { // TODO: Custom formatting?
    cbuffer = new char[12];
    wbuffer = nullptr;
    cCapacity = 12;
    snprintf(cbuffer, 12, "%d", i);
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(int64_t i) {
    cbuffer = new char[21];
    wbuffer = nullptr;
    cCapacity = 21;
    snprintf(cbuffer, 21, "%lld", i);
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(uint32_t i, bool hex) {
    wbuffer = nullptr;
    if (hex) {
        cbuffer = new char[11];
        cCapacity = 11;
        snprintf(cbuffer, 11, "%#010x", i);
    } else {
        cbuffer = new char[12];
        cCapacity = 12;
        snprintf(cbuffer, 12, "%u", i);
    }
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(uint64_t i, bool hex) {
    wbuffer = nullptr;
    if (hex) {
        cbuffer = new char[19];
        cCapacity = 19;
        snprintf(cbuffer, 19, "%#010llx", i);
    } else {
        cbuffer = new char[22];
        cCapacity = 22;
        snprintf(cbuffer, 22, "%llu", i);
    }
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(float f) {
    cbuffer = new char[32];
    wbuffer = nullptr;
    cCapacity = 32;
    snprintf(cbuffer, 32, "%f", f);
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(double d) {
    cbuffer = new char[64];
    wbuffer = nullptr;
    cCapacity = 64;
    snprintf(cbuffer, 64, "%lf", d);
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String& String::operator=(const String& other) {
    if (&other == this) return *this;
    if (wbuffer!=nullptr) { delete[] wbuffer; }
    if (cbuffer!=nullptr) { delete[] cbuffer; }
    wbuffer = new wchar[other.wCapacity];
    cbuffer = new char[other.cCapacity];
    memcpy(wbuffer,other.wstr(),(other.size()+1)*sizeof(wchar));
    memcpy(cbuffer,other.cstr(),(other.size()+1)*sizeof(char));
    wCapacity = other.wCapacity;
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
    if (size() != other.size()) { return false; }
    if (equals(other)) { return true; }
    for (int i = 0; i<strSize; i++) {
        wchar w1 = towlower(wbuffer[i]);
        wchar w2 = towlower(other.wbuffer[i]);
        if (w1 != w2) { return false; }
    }
    return true;
}

bool String::isEmpty() const {
    return strSize == 0;
}

void String::syncBuffers() {
    if (dominantBuffer == DOMINANT_BUFFER::C) {
        utf8ToWChar();
    } else if (dominantBuffer == DOMINANT_BUFFER::W) {
        wCharToUtf8();
    }

    hashCode = 5381;
    for (int i=0;i<strSize;i++) {
        hashCode = ((hashCode << 5) + hashCode) + cbuffer[i];
    }
}

static int measureCodepoint(char chr) {
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

void String::utf8ToWChar() {
    if (wbuffer != nullptr) { delete[] wbuffer; }

    //determine length of the string by measuring all of the codepoints
    strSize = 0;
    for (int i = 0; i < cCapacity;) {
        if (cbuffer[i] == '\0') {
            break;
        } else {
            int codepointLen = measureCodepoint(cbuffer[i]);
            i += codepointLen;
            strSize++;
        }
    }

    wCapacity = strSize + 1;
    wbuffer = new wchar[wCapacity];

    //convert all the codepoints to wchars
    int wIndex = 0;
    for (int i = 0; i < cCapacity;) {
        if (cbuffer[i] == '\0') {
            wbuffer[wIndex] = L'\0';
            break;
        } else {
            int codepointLen = measureCodepoint(cbuffer[i]);

            if (codepointLen == 1) {
                wbuffer[wIndex] = cbuffer[i];
            } else {
                //decode first byte by skipping all bits that indicate the length of the codepoint
                wchar newChar = cbuffer[i] & (0x7f >> codepointLen);
                for (int j = 1; j < codepointLen; j++) {
                    //decode all of the following bytes, fixed 6 bits per byte
                    newChar = (newChar << 6) | (cbuffer[i + j] & 0x3f);
                }
                wbuffer[wIndex] = newChar;
            }

            i += codepointLen;
            wIndex++;
        }
    }
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

void String::wCharToUtf8() {
    if (cbuffer != nullptr) { delete[] cbuffer; }

    //determine the capacity of the cbuffer by measuring the number of bytes required for each codepoint
    strSize = 0;
    cCapacity = 1;
    for (int i = 0; i < wCapacity;i++) {
        if (wbuffer[i] == L'\0') {
            break;
        } else {
            strSize++;
            cCapacity += convertWCharToUtf8(wbuffer[i], nullptr);
        }
    }

    //convert all the wchars to codepoints
    cbuffer = new char[cCapacity];
    char tempBuf[8];
    int cIndex = 0;
    for (int i = 0; i < wCapacity;i++) {
        if (wbuffer[i] == L'\0') {
            cbuffer[cIndex] = '\0';
            break;
        } else {
            int increment = convertWCharToUtf8(wbuffer[i], tempBuf);
            for (int j = 0; j < increment; j++) {
                cbuffer[cIndex] = tempBuf[j];
                cIndex++;
            }
        }
    }
}

int String::size() const {
    return strSize;
}

int String::byteSize() const {
    return (int)strlen(cstr());
}

int String::findFirst(const String& fnd, int from) const {
    if (fnd.size() == 0) { return -1; }
    if (from<0) { from = 0; }
    for (int i = from; i <= size()-fnd.size(); i++) {
        if (memcmp(fnd.wstr(), wstr() + i, fnd.size() * sizeof(wchar)) == 0) { return i; }
    }
    return -1;
}

int String::findLast(const String& fnd, int from) const {
    if (fnd.size() == 0) { return -1; }
    if (from<0) { from = size(); }
    for (int i = from - fnd.size(); i >= 0; i--) {
        if (memcmp(fnd.wstr(), wstr() + i, fnd.size() * sizeof(wchar)) == 0) { return i; }
    }
    return -1;
}

const char* String::cstr() const {
    return cbuffer;
}

const wchar* String::wstr() const {
    return wbuffer;
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

    wchar* newBuf = new wchar[cnt+1];
    newBuf[cnt]=L'\0';
    memcpy(newBuf,wstr()+start,cnt*sizeof(wchar));

    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

char String::charAt(int pos) const {
    return (char)(wstr()[pos]);
}

String String::replace(const String& fnd, const String& rplace) const {
    if (fnd.size()==0) { return *this; }

    int newSize = strSize;
    for (int i=0;i<strSize-fnd.size()+1;) {
        if (memcmp(fnd.wstr(),wstr()+i,fnd.size()*sizeof(wchar))==0) {
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
            found = memcmp(fnd.wstr(),wstr()+i,fnd.size()*sizeof(wchar))==0;
        }
        if (found) {
            memcpy(newBuf+j,rplace.wstr(),rplace.size()*sizeof(wchar));
            i+=fnd.size(); j+=rplace.size();
        } else {
            newBuf[j]=wstr()[i];
            i++; j++;
        }
    }
    newBuf[j]=L'\0';

    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::toUpper() const {
    wchar* newBuf = new wchar[wCapacity];
    for (int i = 0; i<strSize; i++) {
        newBuf[i] = towupper(wbuffer[i]);
    }
    newBuf[strSize] = L'\0';

    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::toLower() const {
    wchar* newBuf = new wchar[wCapacity];
    for (int i = 0; i<strSize; i++) {
        newBuf[i] = towlower(wbuffer[i]);
    }
    newBuf[strSize] = L'\0';

    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::trim() const {
    if (size()==0) { return ""; }

    wchar* newBuf = new wchar[wCapacity];
    int leadingPos = 0;
    while (charAt(leadingPos) == ' ' || charAt(leadingPos) == '\t') {
        leadingPos++;
        if (leadingPos>=size()) {
            delete[] newBuf;
            return *this;
        }
    }
    int trailingPos = size()-1;
    while (charAt(trailingPos) == ' ' || charAt(trailingPos) == '\t') {
        trailingPos--;
        if (trailingPos<0) {
            delete[] newBuf;
            return *this;
        }
    }

    int newLength = 1;
    newBuf[0] = charAt(leadingPos);
    for (int i=leadingPos+1;i<=trailingPos;i++) {
        if (charAt(i-1)!=' ' || charAt(i)!=' ') {
            newBuf[newLength]=charAt(i);
            newLength++;
        }
    }
    newBuf[newLength]=L'\0';

    String retVal = newBuf;
    delete[] newBuf;
    return retVal;
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

    wchar resultBuffer[64];
    int resultSize = 0;

    int tempAscVal = 0;
    for (int i=0;i<size();i++) {
        if (isUnhexing==0) {
            if (wbuffer[i]==L'%') {
                isUnhexing = 1;
            } else {
                resultBuffer[resultSize]=wbuffer[i];
                resultSize++;
            }
        } else {
            int toDec = 0;
            if (wbuffer[i]>=L'0' && wbuffer[i]<=L'9') {
                toDec = wbuffer[i]-L'0';
            } else if (wbuffer[i]>=L'a' && wbuffer[i]<=L'f') {
                toDec = wbuffer[i]+10-L'a';
            } else if (wbuffer[i]>=L'A' && wbuffer[i]<=L'F') {
                toDec = wbuffer[i]+10-L'A';
            }
            if (isUnhexing==1) {
                tempAscVal = toDec<<4;
                isUnhexing=2;
            } else if (isUnhexing==2) {
                tempAscVal |= toDec;
                resultBuffer[resultSize]=tempAscVal;
                resultSize++;
                isUnhexing=0;
            }
        }
    }
    resultBuffer[resultSize]='\0';
    return String(resultBuffer);
}

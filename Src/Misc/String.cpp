#include <Misc/String.h>

#include <cstring>
#include <cwchar>
#include <cstdlib>
#include <cwctype>
#include <iostream>
#ifdef __APPLE__
#ifdef __OBJC__
#import <Foundation/Foundation.h>
#endif
#endif

using namespace PGE;

String::~String() {
    if (cbuffer!=nullptr) { delete[] cbuffer; }
    if (wbuffer!=nullptr) { delete[] wbuffer; }
}

String::String() {
    cbuffer = new char[24];
    wbuffer = nullptr;
    capacity = 24;
    cbuffer[0]='\0';
    strSize = 0;
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(const String& a) {
    int len = a.size();
    wbuffer = new wchar[len+1];
    cbuffer = nullptr;
    capacity = len+1;
    memcpy(wbuffer,a.wstr(),(a.size()+1)*sizeof(wchar));
    strSize = len;
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

String::String(const char* cstri) {
    int len = (int)strlen(cstri);
    cbuffer = new char[len+1];
    wbuffer = nullptr;
    capacity = len+1;
    memcpy(cbuffer, cstri, (len+1)*sizeof(char));
    strSize = len;
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(const std::string& cppstr) {
    int len = (int)cppstr.size();
    cbuffer = new char[len+1];
    wbuffer = nullptr;
    capacity = len+1;
    memcpy(cbuffer, cppstr.c_str(), (len+1)*sizeof(char));
    strSize = len;
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(const wchar* wstri) {
    int len = (int)wcslen(wstri);
    wbuffer = new wchar[len+1];
    cbuffer = nullptr;
    capacity = len+1;
    memcpy(wbuffer, wstri, (len+1)*sizeof(wchar));
    strSize = len;
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

String::String(const std::wstring& cppwstr) {
    int len = (int)cppwstr.size();
    wbuffer = new wchar[len+1];
    cbuffer = nullptr;
    capacity = len+1;
    memcpy(wbuffer, cppwstr.c_str(), (len+1)*sizeof(wchar));
    strSize = len;
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

String::String(const String& a,const String& b) {
    int len = a.size()+b.size();
    wbuffer = new wchar[len+1];
    cbuffer = nullptr;
    capacity = len+1;
    memcpy(wbuffer, a.wstr(), a.size() * sizeof(wchar));
    memcpy(wbuffer+a.size(), b.wstr(), (b.size()+1) * sizeof(wchar));
    strSize = len;
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

String::String(char c) {
    cbuffer = new char[2];
    wbuffer = nullptr;
    capacity = 2;
    cbuffer[0] = c; cbuffer[1]='\0';
    strSize = 1;
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(wchar w) {
    wbuffer = new wchar[2];
    cbuffer = nullptr;
    capacity = 2;
    wbuffer[0] = w; wbuffer[1]=L'\0';
    strSize = 1;
    dominantBuffer = DOMINANT_BUFFER::W;
    syncBuffers();
}

String::String(int i,bool hex) {
    cbuffer = new char[32];
    wbuffer = nullptr;
    capacity = 32;
    if (hex) {
        snprintf(cbuffer,32,"%#010x",i);
    } else {
        snprintf(cbuffer,32,"%d",i);
    }
    strSize = (int)strlen(cbuffer);
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String::String(float f) {
    cbuffer = new char[32];
    wbuffer = nullptr;
    capacity = 32;
    snprintf(cbuffer,32,"%f",f);
    strSize = (int)strlen(cbuffer);
    dominantBuffer = DOMINANT_BUFFER::C;
    syncBuffers();
}

String& String::operator=(const String& other) {
    if (&other == this) return *this;
    if (wbuffer!=nullptr) { delete[] wbuffer; }
    if (cbuffer!=nullptr) { delete[] cbuffer; }
    wbuffer = new wchar[other.capacity];
    cbuffer = new char[other.capacity];
    memcpy(wbuffer,other.wstr(),(other.size()+1)*sizeof(wchar));
    memcpy(cbuffer,other.cstr(),(other.size()+1)*sizeof(char));
    capacity = other.capacity;
    strSize = other.size();
    hashCode = other.getHashCode();
    return *this;
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

bool String::isEmpty() const {
    return strSize == 0;
}

void String::syncBuffers() {
    //TODO: do UTF-8<->WCHAR conversion
    if (dominantBuffer == DOMINANT_BUFFER::C) {
        if (wbuffer!=nullptr) { delete[] wbuffer; }
        wbuffer = new wchar[capacity];
        for (int i=0;i<size()+1;i++) {
            wbuffer[i] = cbuffer[i];
        }
    } else if (dominantBuffer == DOMINANT_BUFFER::W) {
        if (cbuffer!=nullptr) { delete[] cbuffer; }
        cbuffer = new char[capacity];
        for (int i=0;i<size()+1;i++) {
            cbuffer[i] = (char)wbuffer[i];
        }
    }

    hashCode = 5381;
    for (int i=0;i<strSize;i++) {
        hashCode = ((hashCode << 5) + hashCode) + cbuffer[i];
    }
}

int String::size() const {
    return strSize;
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

#ifdef __APPLE__
#ifdef __OBJC__
NSString* String::nsstr() const {
    return [NSString stringWithUTF8String: cbuffer];
}
#endif
#endif

int String::toInt() const {
    return atoi(cbuffer);
}

float String::toFloat() const {
    return (float)atof(cbuffer);
}

String String::substr(int start,int cnt) const {
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

String String::replace(const String& fnd,const String& rplace) const {
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
    wchar* newBuf = new wchar[capacity];
    for (int i = 0; i<strSize; i++) {
        newBuf[i] = towupper(wbuffer[i]);
    }
    newBuf[strSize] = L'\0';

    String retVal(newBuf);
    delete[] newBuf;
    return retVal;
}

String String::toLower() const {
    wchar* newBuf = new wchar[capacity];
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

    wchar* newBuf = new wchar[capacity];
    int leadingPos = 0;
    while (charAt(leadingPos) == ' ' || charAt(leadingPos) == '\t') {
        leadingPos++;
        if (leadingPos>=size()) {
            return *this;
        }
    }
    int trailingPos = size()-1;
    while (charAt(trailingPos) == ' ' || charAt(trailingPos) == '\t') {
        trailingPos--;
        if (trailingPos<0) {
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

String String::resourcePath() const {
#ifdef __APPLE__
#ifdef __OBJC__
    String dummyName = "Dummy.txt";
    int period = dummyName.findFirst(".");
    NSString* name = dummyName.substr(0, period).nsstr();
    NSString* ext = dummyName.substr(period+1).nsstr();

    NSBundle* bundle = [NSBundle mainBundle];
    NSString* path = [bundle pathForResource: name ofType: ext];

    if (path == nullptr) {
        return String("");
    }
    
    // Manipulate the resulting cString.
    const char* cPath = [path cStringUsingEncoding: NSUTF8StringEncoding];
    String strPath = String(cPath);
    strPath = strPath.substr(0, strPath.size() - dummyName.size());
    strPath = strPath + (*this);
    
    return strPath;
#endif
#endif
    return *this;
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

#ifndef PGE_STRING_H_INCLUDED
#define PGE_STRING_H_INCLUDED

#include <vector>
#include <string>

#ifdef __OBJC__
#include <Foundation/NSString.h>
#endif

namespace PGE {

typedef wchar_t wchar;

struct String {
public:
    ~String();
    String();
    String(const String& a);
    String(const char* cstr);
    String(const std::string& cppstr);
    String(const wchar* wstr);
    String(const std::wstring& cppwstr);
    String(const String& a,const String& b);
    String(char c);
    String(wchar w);
    String(int i);
    String(float f);

    const char* cstr() const;
    const wchar* wstr() const;
#ifdef __OBJC__
    NSString* nsstr() const;
#endif
    int toInt() const;
    float toFloat() const;

    int size() const;

    int findFirst(const String& fnd,int from=-1) const;
    int findLast(const String& fnd,int from=-1) const;

    String substr(int start,int cnt=-1) const;
    char charAt(int pos) const;
    String replace(const String& fnd,const String& rplace) const;
    String toUpper() const;
    String toLower() const;
    String trim() const;
    String unHex() const;
    String resourcePath() const;

    String& operator=(const String& other);

    long long getHashCode() const;
    bool equals(const String& other) const;
    bool isEmpty() const;
protected:
    enum class DOMINANT_BUFFER {
        C, W
    } dominantBuffer;

    char* cbuffer = nullptr;
    wchar* wbuffer = nullptr;
    long long hashCode;
    int capacity = 16;
    int strSize = 0;

    void syncBuffers();
};

const String operator+(const String& a, const String& b);
const String operator+(const char* a, const String& b);
bool operator<(const String& a, const String& b);
bool operator>(const String& a, const String& b);
std::ostream& operator<<(std::ostream &os, const String& s);

}

#endif

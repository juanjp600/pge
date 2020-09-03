#ifndef PGE_STRING_H_INCLUDED
#define PGE_STRING_H_INCLUDED

#include <vector>
#include <string>
#include <regex>

#if defined(__APPLE__) && defined(__OBJC__)
#include <Foundation/NSString.h>
#endif

namespace PGE {

typedef wchar_t wchar;

class String {
    public:
        ~String();
        String();
        String(const String& a);
        String(const char* cstr);
        String(const std::string& cppstr);
        String(const wchar* wstr);
        String(const std::wstring& cppwstr);
        #if defined(__APPLE__) && defined(__OBJC__)
        String(const NSString* nsstr);
        #endif
        String(const String& a, const String& b);
        String(char c);
        String(wchar w);

        template <class T>
        static String format(T t, const String& format);
        static String fromInt(int i);
        static String fromFloat(float f);

        const char* cstr() const;
        void wstr(wchar* buffer) const;
        int toInt(bool& success) const;
        float toFloat(bool& success) const;
        int toInt() const;
        float toFloat() const;

        int length() const;
        int byteLength() const;

        int findFirst(const String& fnd, int from=-1) const;
        int findLast(const String& fnd, int from=-1) const;

        String substr(int start, int cnt=-1) const;
        wchar charAt(int pos) const;
        String replace(const String& fnd, const String& rplace) const;
        String toUpper() const;
        String toLower() const;
        String trim() const;
        std::vector<String> split(const String& needle, bool removeEmptyEntries) const;
        static String join(const std::vector<String>& vect, const String& separator);

        std::cmatch regexMatch(const std::regex& pattern) const;

        String unHex() const;

        String& operator=(const String& other);
        String& operator+=(const String& other);

        long long getHashCode() const;
        bool equals(const String& other) const;
        bool equalsIgnoreCase(const String& other) const;
        bool isEmpty() const;

    private:
        String(int size);

        long long hashCode;
        int cCapacity = 0;
        int strByteLength;
        int strLength;

        const static int shortStrCapacity = 16;

        union _StringData {
            char shortStr[shortStrCapacity];
            char* longStr;
            _StringData();
        } data;

        void recalculateHashAndLength();
        void wCharToUtf8Str(const wchar* wbuffer);
        static wchar utf8ToWChar(const char* cbuffer);
        void reallocate(int size);
        char* cstrNoConst();
    };

    bool operator==(const String& a, const String& b);
    bool operator!=(const String& a, const String& b);
    const String operator+(const String& a, const String& b);
    const String operator+(const char* a, const String& b);
    bool operator<(const String& a, const String& b);
    bool operator>(const String& a, const String& b);
    std::ostream& operator<<(std::ostream& os, const String& s);

}

#endif

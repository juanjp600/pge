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
        class Key;
        class RedundantKey;
        class SafeKey;
        class OrderedKey;

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
        String(char c);
        String(wchar w);

        template <class T>
        static String format(T t, const String& format);
        static String fromInt(int i);
        static String fromFloat(float f);

        String& operator=(const String& other);
        String& operator+=(const String& other);
        String& operator+=(wchar ch);

        const char* cstr() const;
        void wstr(wchar* buffer) const;
        int toInt(bool& success) const;
        float toFloat(bool& success) const;
        int toInt() const;
        float toFloat() const;

        int length() const;
        int byteLength() const;

        int findFirst(const String& fnd, int from = -1) const;
        int findLast(const String& fnd, int from = -1) const;

        String substr(int start, int cnt = -1) const;
        wchar charAt(int pos) const;
        String replace(const String& fnd, const String& rplace) const;
        String toUpper() const;
        String toLower() const;
        String trim() const;
        std::vector<String> split(const String& needleStr, bool removeEmptyEntries) const;
        static String join(const std::vector<String>& vect, const String& separator);

        std::cmatch regexMatch(const std::regex& pattern) const;

        //String unHex() const;

        bool equals(const String& other) const;
        bool equalsIgnoreCase(const String& other) const;
        bool isEmpty() const;

    private:
        String(int size);
        String(const String& other, int from, int cnt);

        int cCapacity = 0;

        // Lazily evaluated.
        mutable bool _hashCodeEvaluted;
        mutable uint64_t _hashCode;
        mutable int _strByteLength;
        mutable int _strLength;

        constexpr static int shortStrCapacity = 16;

        union {
            char shortStr[shortStrCapacity];
            char* longStr;
        } data;

        uint64_t getHashCode() const;

        void wCharToUtf8Str(const wchar* wbuffer);
        void reallocate(int size, bool copyOldData = false);
        char* cstrNoConst();

        // TODO: Remove (juan hates his friends).
        friend const String operator+(const String& a, const String& b);
        friend const String operator+(const char* a, const String& b);
        friend const String operator+(const String& a, wchar b);
    };

    const String operator+(const String& a, const String& b);
    const String operator+(const char* a, const String& b);
    const String operator+(const String& a, wchar b);
    bool operator==(const String& a, const String& b);
    bool operator!=(const String& a, const String& b);
    std::ostream& operator<<(std::ostream& os, const String& s);

}

#endif // PGE_STRING_H_INCLUDED

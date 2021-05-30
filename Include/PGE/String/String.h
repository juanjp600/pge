#ifndef PGE_STRING_H_INCLUDED
#define PGE_STRING_H_INCLUDED

#include <unordered_map>
#include <vector>
#include <string>
#include <regex>

#include <PGE/Types/Types.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/NSString.h>
#endif

namespace PGE {

// We always want 16 bits, but Windows functions expect wchar_t, so this only acts as to avoid unnecessary casting.
// wchar_t is always 16 bits on Windows.
#ifdef _WIN32
typedef wchar_t wchar;
#else
typedef char16_t wchar;
#endif

class String {
    public:
        class Key;
        class RedundantKey;
        class SafeKey;
        class OrderedKey;

        struct Iterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = unsigned;
            using value_type = wchar;
            using pointer = value_type*;
            using reference = value_type&;

            Iterator();
            Iterator(const String& str);

            Iterator& operator++();
            Iterator operator++(int);

            Iterator operator+(int steps);
            void operator+=(int steps);

            wchar operator*() const;

            // Member avoids friend.
            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;

            int getPosition() const;

            private:
                Iterator(const String& str, int byteIndex, int chIndex);

                void genChar() const;

                const String* ref;
                // Lazily evaluated, Unicode invalid character by default.
                mutable wchar _ch = L'\uFFFF';
                int index;
                int charIndex;

            friend String;
        };

        Iterator begin() const;
        Iterator end() const;

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

        void operator=(const String& other);
        void operator+=(const String& other);
        void operator+=(wchar ch);

        // TODO: Remove (juan hates his friends).
        friend const String operator+(const String& a, const String& b);
        friend const String operator+(const char* a, const String& b);
        friend const String operator+(const String& a, wchar b);

        const char* cstr() const;
        void wstr(wchar* buffer) const;
        int toInt(bool& success) const;
        float toFloat(bool& success) const;
        int toInt() const;
        float toFloat() const;

        int length() const;
        int byteLength() const;

        Iterator findFirst(const String& fnd, int from = 0) const;
        Iterator findFirst(const String& fnd, const Iterator& from) const;
        Iterator findLast(const String& fnd, int from = 0) const;
        Iterator findLast(const String& fnd, const Iterator& from) const;

        String substr(int start) const;
        String substr(int start, int cnt) const;
        String substr(const Iterator& start) const;
        String substr(const Iterator& start, const Iterator& to) const;
        Iterator charAt(int pos) const;
        String replace(const String& fnd, const String& rplace) const;
        String toUpper() const;
        String toLower() const;
        String trim() const;
        String reverse() const;
        String multiply(int count, const String& separator = "") const;
        std::vector<String> split(const String& needleStr, bool removeEmptyEntries) const;
        static String join(const std::vector<String>& vect, const String& separator);

        std::cmatch regexMatch(const std::regex& pattern) const;

        //String unHex() const;

        u64 getHashCode() const;

        bool equals(const String& other) const;
        bool equalsIgnoreCase(const String& other) const;
        bool isEmpty() const;

    private:
        String(int size);
        String(const String& other, int from, int cnt);

        // Lazily evaluated.
        mutable bool _hashCodeEvaluted;
        mutable u64 _hashCode;
        mutable int _strLength;

        int strByteLength = -1;

        constexpr static int shortStrCapacity = 16;
        int cCapacity = shortStrCapacity;

        union {
            char shortStr[shortStrCapacity];
            char* longStr;
        } data;

        String performCaseConversion(const std::unordered_map<wchar, wchar>& conv, const std::unordered_map<wchar, std::vector<wchar>>& multiConv) const;

        void wCharToUtf8Str(const wchar* wbuffer);
        void reallocate(int size, bool copyOldData = false);
        char* cstrNoConst();
};

bool operator==(const String& a, const String& b);
bool operator!=(const String& a, const String& b);
std::ostream& operator<<(std::ostream& os, const String& s);
std::istream& operator>>(std::istream& is, String& s);

}

#endif // PGE_STRING_H_INCLUDED

#ifndef PGE_STRING_H_INCLUDED
#define PGE_STRING_H_INCLUDED

#include <unordered_map>
#include <vector>
#include <string>
#include <regex>
#include <variant>

#include <PGE/Types/Types.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/NSString.h>
#endif

namespace PGE {

/// A UTF-8 character sequence guaranteed to be terminated by a null byte.
class String {
    public:
        class Key;
        class RedundantKey;
        class SafeKey;
        class OrderedKey;

        class Iterator {
            public:
                Iterator();
                Iterator(const String& str);

                void increment();
                Iterator& operator++();
                const Iterator operator++(int);

                const Iterator operator+(int steps) const;
                void operator+=(int steps);

                char32_t operator*() const;

                bool operator==(const Iterator& other) const;
                bool operator!=(const Iterator& other) const;

                int getPosition() const;

            private:
                Iterator(const String& str, int byteIndex, int chIndex);

                void genChar() const;

                const String* ref;
                // Lazily evaluated, Unicode invalid character by default.
                mutable char32_t _ch = L'\uFFFF';
                int index;
                int charIndex;

            friend String;
        };

        const Iterator begin() const;
        const Iterator end() const;

        ~String() = default; // WHY THE FUCK DO WE NEED THIS??
        static void copy(String& dst, const String& src);
        
        constexpr String() {
            // Manual metadata:
            data->strByteLength = 0;
            data->_strLength = 0;
            data->_hashCode = FNV_SEED;
            data->_hashCodeEvaluted = true;
            cstrNoConst()[0] = '\0';
        }

        String(const String& other);
        String(const char* cstri);

        String(const std::string& cppstr);
        String(const wchar_t* wstr);
        String(const char32_t* wstr);
#if defined(__APPLE__) && defined(__OBJC__)
        String(const NSString* nsstr);
#endif
        String(char c);
        String(char32_t w);

        template <class T>
        static const String format(T t, const String& format);
        static const String fromInt(int i);
        static const String fromFloat(float f);

        void operator=(const String& other);
        void operator+=(const String& other);
        void operator+=(char32_t ch);

        static const String concat(const String& a, const String& b);

        /// Gets the UTF-8 data the string is composd of, without transferring ownership.
        /// Guaranteed to have a null byte appended to the string's content.
        /// 
        /// O(1)
        constexpr const char* cstr() const { return chs; }
        void wstr(char32_t* buffer) const;
        int toInt(bool& success) const;
        float toFloat(bool& success) const;
        int toInt() const;
        float toFloat() const;

        /// The amount of characters the string is composed of, excluding the terminating null byte.
        /// 
        /// First call on same data: O(n), successive calls: O(1)
        int length() const;
        /// The amount of bytes the string is composed of, excluding the terminating null byte.
        /// Not necessarily equal to #length() when dealing with non-ASCII characters.
        /// 
        /// O(1)
        int byteLength() const;

        const Iterator findFirst(const String& fnd, int from = 0) const;
        const Iterator findFirst(const String& fnd, const Iterator& from) const;
        const Iterator findLast(const String& fnd, int from = 0) const;
        const Iterator findLast(const String& fnd, const Iterator& from) const;

        const String substr(int start) const;
        const String substr(int start, int cnt) const;
        const String substr(const Iterator& start) const;
        const String substr(const Iterator& start, const Iterator& to) const;
        const Iterator charAt(int pos) const;
        const String replace(const String& fnd, const String& rplace) const;
        const String toUpper() const;
        const String toLower() const;
        const String trim() const;
        const String reverse() const;
        const String multiply(int count, const String& separator = "") const;
        std::vector<String> split(const String& needleStr, bool removeEmptyEntries) const;
        static const String join(const std::vector<String>& vect, const String& separator);

        const std::cmatch regexMatch(const std::regex& pattern) const;

        //String unHex() const;

        u64 getHashCode() const;

        bool equals(const String& other) const;
        bool equalsIgnoreCase(const String& other) const;
        constexpr bool isEmpty() const { return chs[0] == '\0'; }

    private:
        static constexpr u64 FNV_SEED = 0xcbf29ce484222325;

        String(int size);
        String(const String& other, int from, int cnt);

        static constexpr int SHORT_STR_CAPACITY = 16;

        struct Data {
            // Lazily evaluated.
            mutable bool _hashCodeEvaluted = false;
            mutable u64 _hashCode;
            mutable int _strLength = -1;

            int strByteLength = -1;

            int cCapacity = SHORT_STR_CAPACITY;
        };

        struct Shared {
            Data data;
            std::unique_ptr<char[]> chs;
        };

        struct Unique {
            Data data;
            char chs[SHORT_STR_CAPACITY];
        };

        // Default initialized with Unique.
        std::variant<Unique, std::shared_ptr<Shared>> internalData;
        char* chs = std::get<Unique>(internalData).chs;
        Data* data = &std::get<Unique>(internalData).data;

        const String performCaseConversion(const std::unordered_map<char32_t, char32_t>& conv, const std::unordered_map<char32_t, std::vector<char32_t>>& multiConv) const;

        void wCharToUtf8Str(const char32_t* wbuffer);
        void reallocate(int size, bool copyOldChs = false);
        constexpr char* cstrNoConst() { return chs; }
};
bool operator==(const String& a, const String& b);
bool operator!=(const String& a, const String& b);
const String operator+(const String& a, const String& b);
const String operator+(const char* a, const String& b);
std::ostream& operator<<(std::ostream& os, const String& s);

}

#endif // PGE_STRING_H_INCLUDED

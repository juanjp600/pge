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
        String(const String& a, const String& b);
        String(char c);
        String(wchar w);

        template <class T>
        static String format(T t, const String& format);
        static String fromInt(int i);
        static String fromFloat(float f);

        String& operator=(const String& other);
        String& operator+=(const String& other);

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
        void reallocate(int size);
        char* cstrNoConst();
    };

    class String::Key {
        public:
            friend std::hash<Key>;
            friend std::equal_to<Key>;

            template <class... Args>
            Key(Args... args) : hash(String(args...).getHashCode()) { }
            Key(const String& str) : hash(str.getHashCode()) { }

        private:
            uint64_t hash;
    };

    class String::RedundantKey {
        public:
            friend std::hash<RedundantKey>;
            friend std::equal_to<RedundantKey>;

            template <class... Args>
            RedundantKey(Args... args) : str(args...) { }
            RedundantKey(const String& str) : str(str) { }

            const String& getStr() const {
                return str;
            }

        private:
            String str;

            uint64_t hash() const {
                return str.getHashCode();
            }
    };

    class String::SafeKey {
        public:
            friend std::hash<SafeKey>;
            friend std::equal_to<SafeKey>;

            template <class... Args>
            SafeKey(Args... args) : str(args...) { }
            SafeKey(const String& str) : str(str) { }

            const String& getStr() const {
                return str;
            }

        private:
            String str;

            uint64_t hash() const {
                return str.getHashCode();
            }
    };

    class String::OrderedKey {
        public:
            template <class... Args>
            OrderedKey(Args... args) : str(args...) { }
            OrderedKey(const String & str) : str(str) { }

            const String& getStr() const {
                return str;
            }

            bool operator<(const OrderedKey& other) const {
                return strcmp(getStr().cstr(), other.getStr().cstr()) < 0;
            }

            bool operator>(const OrderedKey& other) const {
                return strcmp(getStr().cstr(), other.getStr().cstr()) > 0;
            }

        private:
            String str;
    };

    bool operator==(const String& a, const String& b);
    bool operator!=(const String& a, const String& b);
    const String operator+(const String& a, const String& b);
    const String operator+(const char* a, const String& b);
    std::ostream& operator<<(std::ostream& os, const String& s);

}

template <> struct std::hash<PGE::String::Key> {
    size_t operator()(const PGE::String::Key& key) const {
        return key.hash;
    }
};

template <> struct std::equal_to<PGE::String::Key> {
    bool operator()(const PGE::String::Key& a, const PGE::String::Key& b) const {
        return a.hash == b.hash;
    }
};

template <> struct std::hash<PGE::String::RedundantKey> {
    size_t operator()(const PGE::String::RedundantKey& key) const {
        return key.hash();
    }
};

template <> struct std::equal_to<PGE::String::RedundantKey> {
    bool operator()(const PGE::String::RedundantKey& a, const PGE::String::RedundantKey& b) const {
        return a.hash() == b.hash();
    }
};

template <> struct std::hash<PGE::String::SafeKey> {
    size_t operator()(const PGE::String::SafeKey& key) const {
        return key.hash();
    }
};

template <> struct std::equal_to<PGE::String::SafeKey> {
    bool operator()(const PGE::String::SafeKey& a, const PGE::String::SafeKey& b) const {
        if (a.str.byteLength() != b.str.byteLength()) { return false; }
        if (a.str.length() != b.str.length()) { return false; }
        return memcmp(a.str.cstr(), b.str.cstr(), a.str.byteLength()) == 0;
    }
};

#endif // PGE_STRING_H_INCLUDED

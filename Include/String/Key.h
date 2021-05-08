#ifndef PGE_STRING_KEY_H_INCLUDED
#define PGE_STRING_KEY_H_INCLUDED

#include "String.h"

namespace PGE {

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

        bool byteLengthEvaluated() const {
            return str._strByteLength >= 0;
        }

        bool lengthEvaluated() const {
            return str._strLength >= 0;
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
        if (a.byteLengthEvaluated() && b.byteLengthEvaluated() && a.str.byteLength() != b.str.byteLength()) { return false; }
        if (a.lengthEvaluated() && b.lengthEvaluated() && a.str.length() != b.str.length()) { return false; }
        return memcmp(a.str.cstr(), b.str.cstr(), a.str.byteLength()) == 0;
    }
};

#endif // PGE_STRING_KEY_H_INCLUDED

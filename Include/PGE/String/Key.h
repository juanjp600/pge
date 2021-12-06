#ifndef PGE_STRING_KEY_H_INCLUDED
#define PGE_STRING_KEY_H_INCLUDED

#include "String.h"

namespace PGE {

struct String::Key {
    Key() = default;
    Key(const String& str) : hash(str.getHashCode()) { }
    size_t hash;
};

struct String::RedundantKey {
    RedundantKey() = default;
    RedundantKey(const String& str) : hash(str.getHashCode()), str(str) { }
    size_t hash;
    String str;
};

struct String::SafeKey {
    SafeKey() = default;
    SafeKey(const String& str) : hash(str.getHashCode()), str(str) { }
    size_t hash;
    String str;
};

struct String::OrderedKey {
    OrderedKey() = default;
    OrderedKey(const String& str) : str(str) { }
    const String str;
    const std::weak_ordering operator<=>(const OrderedKey& other) const {
        return str.compare(other.str);
    }
};

}

template<> struct std::hash<PGE::String::Key> {
    const size_t operator()(const PGE::String::Key& key) const {
        return key.hash;
    }
};

template<> struct std::equal_to<PGE::String::Key> {
    bool operator()(const PGE::String::Key& a, const PGE::String::Key& b) const {
        return a.hash == b.hash;
    }
};

template<> struct std::hash<PGE::String::RedundantKey> {
    size_t operator()(const PGE::String::RedundantKey& key) const {
        return key.hash;
    }
};

template<> struct std::equal_to<PGE::String::RedundantKey> {
    bool operator()(const PGE::String::RedundantKey& a, const PGE::String::RedundantKey& b) const {
        return a.hash == b.hash;
    }
};

template<> struct std::hash<PGE::String::SafeKey> {
    size_t operator()(const PGE::String::SafeKey& key) const {
        return key.hash;
    }
};

template<> struct std::equal_to<PGE::String::SafeKey> {
    bool operator()(const PGE::String::SafeKey& a, const PGE::String::SafeKey& b) const {
        return a.str == b.str;
    }
};

#endif // PGE_STRING_KEY_H_INCLUDED

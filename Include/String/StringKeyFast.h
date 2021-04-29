#ifndef PGE_STRING_KEY_FAST_H_INCLUDED
#define PGE_STRING_KEY_FAST_H_INCLUDED

#include <String/String.h>

namespace PGE {

class StringKeyFast {
    public:
        template <class... Args>
        StringKeyFast(Args... args) : hash(String(args...).getHashCode()) { }
        StringKeyFast(const String& str) : hash(str.getHashCode()) { }

        const uint64_t hash;
};

}

template <> struct std::hash<PGE::StringKeyFast> {
    size_t operator()(const PGE::StringKeyFast& key) const {
        return key.hash;
    }
};

template <> struct std::equal_to<PGE::StringKeyFast> {
    bool operator()(const PGE::StringKeyFast& a, const PGE::StringKeyFast& b) const {
        return a.hash == b.hash;
    }
};

#endif // PGE_STRING_KEY_FAST_H_INCLUDED

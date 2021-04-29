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

using namespace PGE;

template <> struct std::hash<StringKeyFast> {
    size_t operator()(const StringKeyFast& key) const {
        return key.hash;
    }
};

template <> struct std::equal_to<StringKeyFast> {
    bool operator()(const StringKeyFast& a, const StringKeyFast& b) const {
        return a.hash == b.hash;
    }
};

#endif // PGE_STRING_KEY_FAST_H_INCLUDED

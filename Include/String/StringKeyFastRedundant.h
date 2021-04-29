#ifndef PGE_STRING_KEY_FAST_REDUNDANT_H_INCLUDED
#define PGE_STRING_KEY_FAST_REDUNDANT_H_INCLUDED

#include <String/String.h>

namespace PGE {

class StringKeyFastRedundant {
    public:
        template <class... Args>
        StringKeyFastRedundant(Args... args) : str(String(args...)) { }
        StringKeyFastRedundant(const String& str) : str(str) { }

        const String str;
};

}

using namespace PGE;

template <> struct std::hash<StringKeyFastRedundant> {
    size_t operator()(const StringKeyFastRedundant& key) const {
        return key.str.getHashCode();
    }
};

template <> struct std::equal_to<StringKeyFastRedundant> {
    bool operator()(const StringKeyFastRedundant& a, const StringKeyFastRedundant& b) const {
        return a.str.getHashCode() == b.str.getHashCode();
    }
};

#endif // PGE_STRING_KEY_FAST_REDUNDANT_H_INCLUDED

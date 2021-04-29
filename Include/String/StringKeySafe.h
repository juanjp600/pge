#ifndef PGE_STRING_KEY_SAFE_H_INCLUDED
#define PGE_STRING_KEY_SAFE_H_INCLUDED

#include <String/String.h>

namespace PGE {

class StringKeySafe {
    public:
        template <class... Args>
        StringKeySafe(Args... args) : str(String(args...)) { }
        StringKeySafe(const String& str) : str(str) { }

        const String str;
};

}

using namespace PGE;

template <> struct std::hash<StringKeySafe> {
    size_t operator()(const StringKeySafe& key) const {
        return key.str.getHashCode();
    }
};

template <> struct std::equal_to<StringKeySafe> {
    bool operator()(const StringKeySafe& a, const StringKeySafe& b) const {
        if (a.str.byteLength() != b.str.byteLength()) { return false; }
        if (a.str.length() != b.str.length()) { return false; }
        return memcmp(a.str.cstr(), b.str.cstr(), a.str.byteLength()) == 0;
    }
};

#endif // PGE_STRING_KEY_SAFE_H_INCLUDED

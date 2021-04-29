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

template <> struct std::hash<PGE::StringKeySafe> {
    size_t operator()(const PGE::StringKeySafe& key) const {
        return key.str.getHashCode();
    }
};

template <> struct std::equal_to<PGE::StringKeySafe> {
    bool operator()(const PGE::StringKeySafe& a, const PGE::StringKeySafe& b) const {
        if (a.str.byteLength() != b.str.byteLength()) { return false; }
        if (a.str.length() != b.str.length()) { return false; }
        return memcmp(a.str.cstr(), b.str.cstr(), a.str.byteLength()) == 0;
    }
};

#endif // PGE_STRING_KEY_SAFE_H_INCLUDED

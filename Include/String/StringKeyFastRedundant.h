#ifndef PGE_STRING_KEY_FAST_REDUNDANT_H_INCLUDED
#define PGE_STRING_KEY_FAST_REDUNDANT_H_INCLUDED

#include <String/String.h>

namespace PGE {

class StringKeyFastRedundant {
    public:
        friend std::hash<StringKeyFastRedundant>;
        friend std::equal_to<StringKeyFastRedundant>;

        template <class... Args>
        StringKeyFastRedundant(Args... args) : str(String(args...)) { }
        StringKeyFastRedundant(const String& str) : str(str) { }

        const String& getStr() const {
            return str;
        }

    private:
        String str;

        uint64_t hash() const {
            return String::HashFriend::get(str);
        }
};

}

template <> struct std::hash<PGE::StringKeyFastRedundant> {
    size_t operator()(const PGE::StringKeyFastRedundant& key) const {
        return key.hash();
    }
};

template <> struct std::equal_to<PGE::StringKeyFastRedundant> {
    bool operator()(const PGE::StringKeyFastRedundant& a, const PGE::StringKeyFastRedundant& b) const {
        return a.hash() == b.hash();
    }
};

#endif // PGE_STRING_KEY_FAST_REDUNDANT_H_INCLUDED

#ifndef PGE_STRING_KEY_ORDERED_H_INCLUDED
#define PGE_STRING_KEY_ORDERED_H_INCLUDED

namespace PGE {

class StringKeyOrdered {
    public:
        template <class... Args>
        StringKeyOrdered(Args... args) : str(String(args...)) { }
        StringKeyOrdered(const String& str) : str(str) { }

        const String str;
};

bool operator<(const StringKeyOrdered& a, const StringKeyOrdered& b) {
    return strcmp(a.str.cstr(), b.str.cstr()) < 0;
}

bool operator>(const StringKeyOrdered& a, const StringKeyOrdered& b) {
    return strcmp(a.str.cstr(), b.str.cstr()) > 0;
}

}

#endif // PGE_STRING_KEY_ORDERED_H_INCLUDED

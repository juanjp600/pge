#ifndef PGE_STRING_KEY_ORDERED_H_INCLUDED
#define PGE_STRING_KEY_ORDERED_H_INCLUDED

#include <String/String.h>

namespace PGE {

class StringKeyOrdered {
    public:
        template <class... Args>
        StringKeyOrdered(Args... args) : str(String(args...)) { }
        StringKeyOrdered(const String& str) : str(str) { }

        const String& getStr() const {
            return str;
        }

        bool operator<(const StringKeyOrdered& other) const {
            return strcmp(getStr().cstr(), other.getStr().cstr()) < 0;
        }

        bool operator>(const StringKeyOrdered& other) const {
            return strcmp(getStr().cstr(), other.getStr().cstr()) > 0;
        }

    private:
        String str;
};

}

#endif // PGE_STRING_KEY_ORDERED_H_INCLUDED

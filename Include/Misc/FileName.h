#ifndef PGE_FILENAME_H_INCLUDED
#define PGE_FILENAME_H_INCLUDED

#include "String.h"

namespace PGE {

class FileName {
    private:
        String name;

    public:
        FileName();
        static FileName create(const String& str);
        FileName(const FileName& a, const String& b);

        const String str() const;
        const char* cstr() const;
        const wchar* wstr() const;

        int size() const;

        bool exists() const;

        FileName& operator=(const FileName& other);

        long long getHashCode() const;
        bool equals(const FileName& other) const;
        bool isEmpty() const;
};

const FileName operator+(const FileName& a, const String& b);
std::ostream& operator<<(std::ostream& os, const FileName& fn);

}

#endif

#ifndef PGE_FILEPATH_H_INCLUDED
#define PGE_FILEPATH_H_INCLUDED

#include "String.h"

namespace PGE {

class FilePath {
    private:
        String name;
    
        static String getResourcePath();

    public:
        FilePath();
        static FilePath fromStr(const String& str);
        FilePath(const FilePath& a, const String& b);

        FilePath validateAsDirectory() const;

        const String& str() const;
        const char* cstr() const;
        const wchar* wstr() const;

        String getExtension() const;

        int size() const;

        bool exists() const;

        FilePath& operator=(const FilePath& other);

        long long getHashCode() const;
        bool equals(const FilePath& other) const;
        bool isEmpty() const;
};

const FilePath operator+(const FilePath& a, const String& b);
std::ostream& operator<<(std::ostream& os, const FilePath& fn);

}

#endif

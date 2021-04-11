#ifndef PGE_FILEPATH_H_INCLUDED
#define PGE_FILEPATH_H_INCLUDED

#include "String.h"

namespace PGE {

class FilePath {
    private:
        String name;
        bool valid;
    
        static String getResourcePath();

        static const String INVALID_STR;

    public:
        FilePath();
        static FilePath fromStr(const String& str);
        FilePath(const FilePath& a, const String& b);

        FilePath validateAsDirectory() const;

        const String& str() const;
        const char* cstr() const;
        void wstr(wchar* buffer) const;

        String getExtension() const;

        int byteLength() const;
        int length() const;

        bool exists() const;

        long long getHashCode() const;
        bool equals(const FilePath& other) const;
        bool isValid() const;
};

const FilePath operator+(const FilePath& a, const String& b);
std::ostream& operator<<(std::ostream& os, const FilePath& fn);

}

#endif // PGE_FILEPATH_H_INCLUDED

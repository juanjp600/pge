#ifndef PGE_FILEPATH_H_INCLUDED
#define PGE_FILEPATH_H_INCLUDED

#include <String/String.h>

namespace PGE {

class FilePath {
    private:
        String name;
        bool valid;

        FilePath(const String& str);

    public:
        FilePath();
        static FilePath fromStr(const String& str);
        FilePath(const FilePath& a, const String& b);

        // Causes a path to be interpreted as a directory, does not actually create the directory.
        FilePath makeDirectory() const;

        const String& str() const;
        const char* cstr() const;
        void wstr(wchar* buffer) const;

        String getExtension() const;

        int byteLength() const;
        int length() const;

        bool exists() const;

        uint64_t getHashCode() const;
        bool equals(const FilePath& other) const;
        bool isValid() const;
};

const FilePath operator+(const FilePath& a, const String& b);
std::ostream& operator<<(std::ostream& os, const FilePath& fn);

}

#endif // PGE_FILEPATH_H_INCLUDED

#ifndef PGE_FILEPATH_H_INCLUDED
#define PGE_FILEPATH_H_INCLUDED

#include "String.h"

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

        // OS-dependant folder for storing external files.
        static const FilePath& getDataPath();

        bool isValid() const;

        // Causes a path to be interpreted as a directory, does not actually create the directory.
        FilePath makeDirectory() const;

        String getExtension() const;

        // Check if a path exists on the system.
        bool exists() const;

        // Attempts to create a folder in the given path, returns whether it suceeds.
        bool createDirectory() const;

        // Returns all direct subdirectories.
        void enumerateFolders(std::vector<FilePath>& folders) const;

        // Enumerates subdirectories.
        void enumerateFiles(std::vector<FilePath>& files) const;

        // Returns all lines from a file.
        void readLines(std::vector<String>& lines, bool includeEmptyLines = false) const;

        // Returns all bytes in a file.
        void readBytes(std::vector<uint8_t>& bytes) const;

        int byteLength() const;
        int length() const;

        const String& str() const;
        const char* cstr() const;
        void wstr(wchar* buffer) const;

        bool equals(const FilePath& other) const;
};

const FilePath operator+(const FilePath& a, const String& b);
std::ostream& operator<<(std::ostream& os, const FilePath& fn);

}

#endif // PGE_FILEPATH_H_INCLUDED

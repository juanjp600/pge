#ifndef PGE_FILEUTIL_H_INCLUDED
#define PGE_FILEUTIL_H_INCLUDED

#include <vector>

#include <Misc/FilePath.h>

namespace PGE {

class FileUtil {
    public:
        // Check if a path exists on the system.
        static bool exists(const FilePath& path);

        // Attempts to create a folder in the given path, returns whether it suceeds.
        static bool createDirectory(const FilePath& path);

        // Creates a folder in the given path if it doesn't already exist.
        // Returns -1 if the directory already existed, 0 on failure and 1 on success of creation.
        static int createDirectoryIfNotExists(const FilePath& path);

        // OS-dependant folder for storing external files.
        static const FilePath& getDataPath();

        // Returns all direct subdirectories.
        static void enumerateFolders(const FilePath& path, std::vector<FilePath>& folders);

        // Enumerates subdirectories.
        static void enumerateFiles(const FilePath& path, std::vector<FilePath>& files);

        // Returns all lines from a file.
        static void readLines(const FilePath& path, std::vector<String>& lines, bool includeEmptyLines=false);

        // Returns all bytes in a file.
        static void readBytes(const FilePath& path, std::vector<uint8_t>& bytes);
};
    
}

#endif // PGE_FILEUTIL_H_INCLUDED

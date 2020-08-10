#ifndef FileUtil_H_INCLUDED
#define FileUtil_H_INCLUDED

#include <Misc/FilePath.h>
#include <vector>

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
        static String getDataFolder();

        // Enumerates subdirectories.
        static std::vector<FilePath> enumerateFiles(const FilePath& path);

        // Returns all lines from a file.
        static std::vector<String> readLines(const FilePath& path, bool includeEmptyLines=false);
};
    
}

#endif // FileUtil_H_INCLUDED

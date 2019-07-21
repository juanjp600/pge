#ifndef FileUtil_H_INCLUDED
#define FileUtil_H_INCLUDED

#include <Misc/String.h>

namespace PGE {

class FileUtil {
    public:
        // Check if a path exists on the system.
        static bool exists(String path);

        // Attempts to create a folder in the given path, returns whether it suceeds.
        static bool createDirectory(String path);

        // OS-dependant folder for storing external files.
        static String getDataFolder();
};
    
}

#endif // FileUtil_H_INCLUDED

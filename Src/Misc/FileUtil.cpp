#include <fstream>
#if defined(__APPLE__) && defined(__OBJC__)
    #import <Foundation/Foundation.h>
#endif

#ifdef WINDOWS
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

#include <Misc/FileUtil.h>
#include <Misc/String.h>

using namespace PGE;

bool FileUtil::exists(String path) {
    struct stat buf;
    return (stat(path.cstr(), &buf) == 0);
}

bool FileUtil::createDirectory(String path) {
    if (exists(path)) {
        return false;
    }

#ifdef WINDOWS
    // TODO:
    return false;
#else
    return mkdir(path.cstr(), S_IRWXU) == 0;
#endif
}

String FileUtil::getDataFolder() {
    // TODO: Windows and Linux.
#if defined(__APPLE__) && defined(__OBJC__)
    // Volumes/User/*user*/Library/Application Support/
    NSArray* filePaths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* appSupportDir = [filePaths firstObject];

    return String([appSupportDir cStringUsingEncoding: NSUTF8StringEncoding]) + "/";
#endif

    return PGE::String();
}

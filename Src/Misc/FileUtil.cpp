#include <fstream>

#if defined(__APPLE__) && defined(__OBJC__)
    #import <Foundation/Foundation.h>
#else
    #include <filesystem>
#endif

#ifdef WINDOWS
#include <Windows.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

#include <Misc/FileUtil.h>
#include <Misc/FilePath.h>

using namespace PGE;

bool FileUtil::exists(const FilePath& path) {
#ifdef __APPLE__
    struct stat buf;
    return (stat(path.cstr(), &buf) == 0);
#else
    return std::filesystem::exists(path.cstr());
#endif
}

bool FileUtil::createDirectory(const FilePath& path) {
    if (exists(path)) {
        return false;
    }

#ifdef WINDOWS
    SECURITY_ATTRIBUTES attrs;
    attrs.bInheritHandle = false;
    attrs.nLength = 0;
    if (CreateDirectoryW(path.wstr(), NULL)) {
        return true;
    }
    GetLastError(); //TODO: log this or some shit, idgaf
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

std::vector<FilePath> FileUtil::enumerateFiles(const FilePath& path) {
    std::vector<FilePath> files;
    
#if WINDOWS
    WIN32_FIND_DATAW ffd;
    LARGE_INTEGER filesize;
    size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    FilePath filePath = path;
    if (filePath.str().charAt(filePath.size() - 1) != '/') {
        filePath = FilePath(path, "/");
    }

    FilePath searchQuery = FilePath(filePath, "*");

    hFind = FindFirstFileW(searchQuery.wstr(), &ffd);

    if (hFind == INVALID_HANDLE_VALUE) {
        return files;
    }

    while (true) {
        String fileName = ffd.cFileName;
        if (fileName.equals(".") || fileName.equals(".."))
        {
            if (!FindNextFileW(hFind, &ffd)) {
                break;
            }
            continue;
        }

        FilePath newPath = FilePath(filePath, fileName);
        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            if (newPath.str().charAt(newPath.size() - 1) != '/') {
                newPath = FilePath(newPath, "/");
            }
            std::vector<FilePath> nestedFiles = enumerateFiles(newPath);
            for (int i = 0; i < nestedFiles.size(); i++) {
                files.push_back(nestedFiles[i]);
            }
        } else {
            files.push_back(newPath);
        }

        if (!FindNextFileW(hFind, &ffd)) {
            break;
        }
    }

    FindClose(hFind);
#else
    DIR *dir;
    struct dirent* currentEntry;

    dir = opendir(path.cstr());
    
    if (dir == NULL) {
        return files;
    }
    
    while ((currentEntry = readdir(dir)) != NULL) {
        if (currentEntry->d_type == DT_DIR){ //if item is a directory, print its contents
//            char path[1024];
            //skip '.' and '..'
            if ((strcmp(currentEntry->d_name, ".")) != 0 && strcmp(currentEntry->d_name, "..") != 0) {
//                printf("[%s]\n","", currentEntry->d_name);
                FilePath newPath = FilePath(path);
                newPath = FilePath(newPath, currentEntry->d_name);
                if (newPath.str().charAt(newPath.size() - 1) != '/') {
                    newPath = FilePath(newPath, "/");
                }
                std::vector<FilePath> nestedFiles = enumerateFiles(newPath);
                for (int i = 0; i < nestedFiles.size(); i++) {
                    files.push_back(nestedFiles[i]);
                }
            } else {
                continue;
            }
        } else {
            files.push_back(FilePath(path, currentEntry->d_name));
        }
    }
    closedir(dir);

#endif

    return files;
}

std::vector<String> FileUtil::readLines(const FilePath& path, bool includeEmptyLines) {
    std::vector<String> retVal;

    std::ifstream file; file.open(path.cstr(), std::ios_base::in);

    char* tempBuf = new char[1024];

    file.getline(tempBuf, sizeof(char) * 1024);
    while (true) {
        if (file.eof()) { break; }
        String str = tempBuf;
        str = str.replace("\n", "").replace("\r", "");
        file.getline(tempBuf, sizeof(char) * 1024);
        if ((!includeEmptyLines) && (str.size() <= 0)) { continue; }
        retVal.push_back(str);
    }

    delete[] tempBuf;

    file.close();

    return retVal;
}


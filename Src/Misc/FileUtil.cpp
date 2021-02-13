#include <fstream>

#if defined(__APPLE__) && defined(__OBJC__)
    #import <Foundation/Foundation.h>
#else
    #include <filesystem>
#endif

#ifdef WINDOWS
#include <Windows.h>
#include <ShlObj_core.h>
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
    wchar* wstr = new wchar[path.length() + 1];
    path.wstr(wstr);
    bool success = CreateDirectoryW(wstr, NULL);
    delete[] wstr;
    if (success) {
        return true;
    } else {
        throw std::runtime_error(PGE::String::fromInt((int)GetLastError()).cstr());
    }
#else
    return mkdir(path.cstr(), S_IRWXU) == 0;
#endif
}

int FileUtil::createDirectoryIfNotExists(const FilePath& path) {
    if (exists(path)) {
        return -1;
    } else {
        return createDirectory(path);
    }
}

String FileUtil::getDataFolder() {
    // TODO: Linux.
#if defined(__APPLE__) && defined(__OBJC__)
    // Volumes/User/*user*/Library/Application Support/
    NSArray* filePaths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString* appSupportDir = [filePaths firstObject];

    return String([appSupportDir cStringUsingEncoding: NSUTF8StringEncoding]) + "/";
#elif defined(WINDOWS)
    // Users/*user*/AppData/Roaming/ 
    PWSTR filePath;
    SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &filePath);
    PGE::String path = filePath;
    CoTaskMemFree(filePath);
    return path.replace('\\', '/') + '/';
#endif

    return PGE::String();
}

std::vector<FilePath> FileUtil::enumerateFolders(const FilePath& path) {
    std::vector<FilePath> folders;

#if WINDOWS
    HANDLE hFind;
    WIN32_FIND_DATAW ffd;

    FilePath filePath = path.validateAsDirectory();

    FilePath anyPath = filePath + "*";
    wchar* wstr = new wchar[anyPath.length() + 1];
    anyPath.wstr(wstr);
    hFind = FindFirstFileW(wstr, &ffd);
    //delete[] wstr; // TODO Memleak

    if (hFind == INVALID_HANDLE_VALUE) {
        throw std::runtime_error(PGE::String::fromInt((int)GetLastError()).cstr());
    }

    do {
        String fileName = ffd.cFileName;
        if (fileName.equals(".") || fileName.equals("..")) {
            continue;
        }

        FilePath newPath = FilePath(filePath, fileName);
        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            folders.push_back(newPath.validateAsDirectory());
        }
    } while (FindNextFileW(hFind, &ffd));

    FindClose(hFind);
#else
    DIR* dir;
    struct dirent* currentEntry;

    dir = opendir(path.cstr());

    if (dir == NULL) {
        throw std::runtime_error(path.cstr());
    }

    while ((currentEntry = readdir(dir)) != NULL) {
        if (currentEntry->d_type == DT_DIR) {
            // Skip '.' and '..'.
            if ((strcmp(currentEntry->d_name, ".")) != 0 && strcmp(currentEntry->d_name, "..") != 0) {
                folders.push_back(FilePath(path, currentEntry->d_name).validateAsDirectory());
            } else {
                continue;
            }
        }
    }
    closedir(dir);
#endif

    return folders;
}

std::vector<FilePath> FileUtil::enumerateFiles(const FilePath& path) {
    std::vector<FilePath> files;
    
#if WINDOWS
    HANDLE hFind;
    WIN32_FIND_DATAW ffd;

    FilePath filePath = path.validateAsDirectory();

    FilePath anyPath = filePath + "*";
    wchar* wstr = new wchar[anyPath.length() + 1];
    anyPath.wstr(wstr);
    hFind = FindFirstFileW(wstr, &ffd);
    delete[] wstr;

    if (hFind == INVALID_HANDLE_VALUE) {
        throw std::runtime_error(PGE::String::fromInt((int)GetLastError()).cstr());
    }

    do {
        String fileName = ffd.cFileName;
        if (fileName.equals(".") || fileName.equals("..")) {
            continue;
        }

        FilePath newPath = FilePath(filePath, fileName);
        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            std::vector<FilePath> nestedFiles = enumerateFiles(newPath);
            for (int i = 0; i < nestedFiles.size(); i++) {
                files.push_back(nestedFiles[i]);
            }
        } else {
            files.push_back(newPath);
        }
    } while (FindNextFileW(hFind, &ffd));

    FindClose(hFind);
#else
    DIR* dir;
    struct dirent* currentEntry;

    dir = opendir(path.cstr());
    
    if (dir == NULL) {
        throw std::runtime_error(path.cstr());
    }
    
    while ((currentEntry = readdir(dir)) != NULL) {
        if (currentEntry->d_type == DT_DIR) { // If item is a directory, print its contents.
//            char path[1024];
            // Skip '.' and '..'.
            if ((strcmp(currentEntry->d_name, ".")) != 0 && strcmp(currentEntry->d_name, "..") != 0) {
//                printf("[%s]\n","", currentEntry->d_name);
                FilePath newPath = FilePath(path);
                newPath = FilePath(newPath, currentEntry->d_name);
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
        if ((!includeEmptyLines) && (str.length() <= 0)) { continue; }
        retVal.push_back(str);
    }

    delete[] tempBuf;

    file.close();

    return retVal;
}

std::vector<uint8_t> FileUtil::readBytes(const FilePath& path) {
    std::ifstream file;
    file.open(path.cstr(), std::ios::ate | std::ios::binary);
    size_t vertSize = file.tellg();
    std::vector<uint8_t> ret = std::vector<uint8_t>(vertSize);
    file.seekg(0);
    file.read((char*)&ret[0], vertSize);
    file.close();
    return ret;
}

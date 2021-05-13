#include <Misc/FilePath.h>

#include <fstream>

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#else
#include <filesystem>
#endif

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj_core.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

#include <Exception/Exception.h>

using namespace PGE;

static const String INVALID_STR = "Tried using an invalid path";

static String sanitizeFileSeperator(const String& str) {
    return str.replace("\\", "/");
}

static String resourceStr;

// Already sanitized.
static String getResourceStr() {
    if (resourceStr.byteLength() == 0) {
#if defined(__APPLE__) && defined(__OBJC__)
        NSBundle* bundle = [NSBundle mainBundle];
        resourceStr = String(bundle.resourcePath);
#else
        resourceStr = String(std::filesystem::current_path().c_str());
#endif
        resourceStr = sanitizeFileSeperator(resourceStr);
        resourceStr += "/";
    }
    return resourceStr;
}

static FilePath dataPath;

const FilePath& FilePath::getDataPath() {
    if (!dataPath.isValid()) {
        // TODO: Linux.
#if defined(__APPLE__) && defined(__OBJC__)
    // Volumes/User/*user*/Library/Application Support/
        NSArray* filePaths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString* appSupportDir = [filePaths firstObject];

        dataPath = FilePath::fromStr([appSupportDir cStringUsingEncoding : NSUTF8StringEncoding]);
#elif defined(_WIN32)
    // Users/*user*/AppData/Roaming/ 
        PWSTR filePath;
        SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &filePath);
        PGE::String path = filePath;
        CoTaskMemFree(filePath);
        dataPath = FilePath::fromStr(path);
#endif
        dataPath = dataPath.makeDirectory();
    }
    return dataPath;
}

FilePath::FilePath(const String& str) {
    name = str;
    valid = true;
}

FilePath::FilePath() {
    name = "<n/a>";
    valid = false;
}

FilePath FilePath::fromStr(const String& str) {
    std::filesystem::path pth = str.cstr();
    String sanitizedStr = sanitizeFileSeperator(str);
    if (pth.is_absolute()) {
        return FilePath(sanitizedStr);
    } else {
        return FilePath(getResourceStr() + sanitizedStr);
    }
}

FilePath::FilePath(const FilePath& a, const String& b) {
    PGE_ASSERT(a.valid, INVALID_STR);
    name = a.str() + sanitizeFileSeperator(b);
    valid = true;
}

bool FilePath::isValid() const {
    return valid;
}

FilePath FilePath::makeDirectory() const {
    PGE_ASSERT(valid, INVALID_STR);
    if (*str().charAt(length() - 1) != '/') {
        return *this + "/";
    }
    return *this;
}

String FilePath::getExtension() const {
    PGE_ASSERT(valid, INVALID_STR);
    String::Iterator startIndex = name.findLast(".");
    if (startIndex == name.end()) { return ""; }
    return name.substr(startIndex+1);
}

bool FilePath::exists() const {
    PGE_ASSERT(valid, INVALID_STR);
#ifdef __APPLE__
    struct stat buf;
    return (stat(path.cstr(), &buf) == 0);
#else
    return std::filesystem::exists(cstr());
#endif
}

bool FilePath::createDirectory() const {
    if (exists()) {
        return false;
    }

#ifdef _WIN32
    SECURITY_ATTRIBUTES attrs;
    attrs.bInheritHandle = false;
    attrs.nLength = 0;
    wchar* wstr = new wchar[length() + 1];
    str().wstr(wstr);
    bool success = CreateDirectoryW(wstr, NULL);
    delete[] wstr;
    PGE_ASSERT(success, "Couldn't create directory (dir: " + str() + "; err: " + PGE::String::fromInt((int)GetLastError()) + ")");
    return true;
#else
    return mkdir(path.cstr(), S_IRWXU) == 0;
#endif
}

void FilePath::enumerateFolders(std::vector<FilePath>& folders) const {
#if _WIN32
    HANDLE hFind;
    WIN32_FIND_DATAW ffd;

    FilePath filePath = makeDirectory();

    FilePath anyPath = filePath + "*";
    wchar* wstr = new wchar[anyPath.length() + 1];
    anyPath.wstr(wstr);
    hFind = FindFirstFileW(wstr, &ffd);
    delete[] wstr;

    PGE_ASSERT(hFind != INVALID_HANDLE_VALUE, "Couldn't enumerate directory (dir: " + str() + "; err: " + PGE::String::fromInt((int)GetLastError()) + ")");

    do {
        String fileName = ffd.cFileName;
        if (fileName.equals(".") || fileName.equals("..")) {
            continue;
        }

        FilePath newPath = FilePath(filePath, fileName);
        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            folders.push_back(newPath.makeDirectory());
        }
    } while (FindNextFileW(hFind, &ffd));

    FindClose(hFind);
#else
    DIR* dir;
    struct dirent* currentEntry;

    dir = opendir(path.cstr());

    PGE_ASSERT(dir != NULL, "Couldn't open directory (dir: " + str() + ")");

    while ((currentEntry = readdir(dir)) != NULL) {
        if (currentEntry->d_type == DT_DIR) {
            // Skip '.' and '..'.
            if ((strcmp(currentEntry->d_name, ".")) != 0 && strcmp(currentEntry->d_name, "..") != 0) {
                folder.push_back(FilePath(path, currentEntry->d_name).validateAsDirectory());
            } else {
                continue;
            }
        }
    }
    closedir(dir);
#endif
}

void FilePath::enumerateFiles(std::vector<FilePath>& files) const {
#if _WIN32
    HANDLE hFind;
    WIN32_FIND_DATAW ffd;

    FilePath filePath = makeDirectory();

    FilePath anyPath = filePath + "*";
    wchar* wstr = new wchar[anyPath.length() + 1];
    anyPath.wstr(wstr);
    hFind = FindFirstFileW(wstr, &ffd);
    delete[] wstr;

    PGE_ASSERT(hFind != INVALID_HANDLE_VALUE, "Couldn't enumerate directory (dir: " + str() + "; err: " + PGE::String::fromInt((int)GetLastError()) + ")");

    do {
        String fileName = ffd.cFileName;
        if (fileName.equals(".") || fileName.equals("..")) {
            continue;
        }

        FilePath newPath = FilePath(filePath, fileName);
        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            newPath.enumerateFiles(files);
        } else {
            files.push_back(newPath);
        }
    } while (FindNextFileW(hFind, &ffd));

    FindClose(hFind);
#else
    DIR* dir;
    struct dirent* currentEntry;

    dir = opendir(path.cstr());

    PGE_ASSERT(dir != NULL, "Couldn't open directory (dir: " + str() + ")");

    while ((currentEntry = readdir(dir)) != NULL) {
        if (currentEntry->d_type == DT_DIR) { // If item is a directory, print its contents.
//            char path[1024];
            // Skip '.' and '..'.
            if ((strcmp(currentEntry->d_name, ".")) != 0 && strcmp(currentEntry->d_name, "..") != 0) {
                //                printf("[%s]\n","", currentEntry->d_name);
                FilePath newPath = FilePath(path);
                newPath = FilePath(newPath, currentEntry->d_name);
                enumerateFiles(newPath, files);
            } else {
                continue;
            }
        } else {
            files.push_back(FilePath(path, currentEntry->d_name));
        }
    }
    closedir(dir);
#endif
}

void FilePath::readLines(std::vector<String>& lines, bool includeEmptyLines) const {
    std::ifstream file; file.open(cstr(), std::ios_base::in);

    char* tempBuf = new char[1024];

    file.getline(tempBuf, sizeof(char) * 1024);
    while (true) {
        if (file.eof()) { break; }
        String str = tempBuf;
        str = str.replace("\n", "").replace("\r", "");
        file.getline(tempBuf, sizeof(char) * 1024);
        if ((!includeEmptyLines) && (str.length() <= 0)) { continue; }
        lines.push_back(str);
    }

    delete[] tempBuf;

    file.close();
}

void FilePath::readBytes(std::vector<byte>& bytes) const {
    std::ifstream file;
    file.open(cstr(), std::ios::ate | std::ios::binary);
    PGE_ASSERT(file.good(), "File is not good (file: \"" + str() + "\")");
    size_t vertSize = (size_t)file.tellg();
    bytes.resize(bytes.size() + vertSize);
    file.seekg(0);
    file.read((char*)bytes.data(), vertSize);
    file.close();
}

int FilePath::byteLength() const {
    return name.byteLength();
}

int FilePath::length() const {
    return name.length();
}

const String& FilePath::str() const {
    return name;
}

const char* FilePath::cstr() const {
    return name.cstr();
}

void FilePath::wstr(wchar* buffer) const {
    name.wstr(buffer);
}

bool FilePath::equals(const FilePath& other) const {
    // Invalid paths never equal one-another.
    if (!isValid() || !other.isValid()) {
        return false;
    }
    return name.equals(other.name);
}

const FilePath PGE::operator+(const FilePath& a, const String& b) {
    return FilePath(a, b);
}

std::ostream& PGE::operator<<(std::ostream& os, const FilePath& fn) {
    return os.write(fn.cstr(), fn.byteLength());
}

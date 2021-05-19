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

const String INVALID_STR = "Tried using an invalid path";

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

bool FilePath::isDirectory() const {
    PGE_ASSERT(valid, INVALID_STR);
    std::error_code err;
    bool isDir = std::filesystem::is_directory(cstr(), err);
    PGE_ASSERT(err.value() == 0, "Couldn't check if path is directory (dir: " + str() + "; err: " + err.message() + " (" + PGE::String::fromInt(err.value()) + "))");
    return isDir;
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
    std::error_code err;
    bool exists = std::filesystem::exists(cstr(), err);
    PGE_ASSERT(err.value() == 0, "Couldn't check if directory exists (dir: " + str() + "; err: " + err.message() + " (" + PGE::String::fromInt(err.value()) + "))");
    return exists;
}

bool FilePath::createDirectory() const {
    if (exists()) {
        return false;
    }

    std::vector<wchar> wstr = std::vector<wchar>(length() + 1);
    str().wstr(wstr.data());

    std::error_code err;
    bool created = std::filesystem::create_directories(wstr.data(), err);
    PGE_ASSERT(err.value() == 0, "Couldn't create directory (dir: " + str() + "; err: " + err.message() + " (" + PGE::String::fromInt(err.value()) + "))");
    return created;
}

void FilePath::enumerateFolders(std::vector<FilePath>& folders) const {
    for (const auto& it : std::filesystem::directory_iterator(this->cstr())) {
        folders.push_back(FilePath::fromStr(it.path().c_str()));
    }
}

void FilePath::enumerateFiles(std::vector<FilePath>& files) const {
    for (const std::filesystem::directory_entry& it : std::filesystem::recursive_directory_iterator(this->cstr())) {
        files.push_back(FilePath::fromStr(it.path().c_str()));
    }
}

void FilePath::readLines(std::vector<String>& lines, bool includeEmptyLines) const {
    std::ifstream file; file.open(cstr());

    String line;
    while (!file.eof()) {
        line = String();
        file >> line;
        if ((!includeEmptyLines) && (line.length() == 0)) { continue; }
        lines.push_back(line);
    }

    file.close();
}

void FilePath::readBytes(std::vector<byte>& bytes) const {
    std::ifstream file; file.open(cstr(), std::ios::ate | std::ios::binary);
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

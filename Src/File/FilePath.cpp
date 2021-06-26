#include <PGE/File/FilePath.h>

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

#include <PGE/Exception/Exception.h>

#include <PGE/File/FileReader.h>

using namespace PGE;

const String INVALID_STR = "Tried using an invalid path";

static String sanitizeFileSeperator(const String& str) {
    return str.replace("\\", "/");
}

// Already sanitized.
static String& getResourceStr() {
    static String resourceStr;
    if (resourceStr.byteLength() == 0) {
#if defined(__APPLE__) && defined(__OBJC__)
        NSBundle* bundle = [NSBundle mainBundle];
        resourceStr = String(bundle.resourcePath);
#else
        resourceStr = String(std::filesystem::current_path().c_str());
#endif
        resourceStr = sanitizeFileSeperator(resourceStr);
        resourceStr += '/';
    }
    return resourceStr;
}

const FilePath& FilePath::getDataPath() {
    static FilePath dataPath;
    if (!dataPath.isValid()) {
        // TODO: Linux.
#if defined(__APPLE__) && defined(__OBJC__)
        NSArray* filePaths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString* appSupportDir = [filePaths firstObject];

        dataPath = FilePath::fromStr([appSupportDir cStringUsingEncoding : NSUTF8StringEncoding]);
#elif defined(_WIN32)
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

const FilePath FilePath::makeDirectory() const {
    PGE_ASSERT(valid, INVALID_STR);
    if (*str().charAt(name.length() - 1) != '/') {
        return *this + String("/");
    }
    return *this;
}

const FilePath FilePath::up() const {
    PGE_ASSERT(valid, INVALID_STR);
    String::Iterator to = name.findLast("/");
    int index;
    if (to + 1 == name.end()) {
        index = name.substr(name.begin(), to).findLast("/").getPosition();
    } else {
        index = to.getPosition();
    }
    return FilePath(name.substr(0, index + 1));
}

const String FilePath::getExtension() const {
    PGE_ASSERT(valid, INVALID_STR);
    String::Iterator startIndex = name.findLast(".");
    if (startIndex == name.end()) { return ""; }
    return name.substr(startIndex+1);
}

const FilePath FilePath::trimExtension() const {
    PGE_ASSERT(valid, INVALID_STR);
    String::Iterator startIndex = name.findLast(".");
    if (startIndex == name.end()) { return *this; }
    return name.substr(name.begin(), startIndex);
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

    std::vector<char32_t> wstr = std::vector<char32_t>(name.length() + 1);
    str().wstr(wstr.data());

    std::error_code err;
    bool created = std::filesystem::create_directories(wstr.data(), err);
    PGE_ASSERT(err.value() == 0, "Couldn't create directory (dir: " + str() + "; err: " + err.message() + " (" + PGE::String::fromInt(err.value()) + "))");
    return created;
}

std::vector<FilePath> FilePath::enumerateFolders() const {
    std::vector<FilePath> folders;
    for (const auto& it : std::filesystem::directory_iterator(cstr())) {
        if (it.is_directory()) {
            folders.push_back(FilePath::fromStr(it.path().c_str()));
        }
    }
    return folders;
}

std::vector<FilePath> FilePath::enumerateFiles(bool recursive) const {
    std::vector<FilePath> files;
    if (recursive) {
        for (const auto& it : std::filesystem::recursive_directory_iterator(cstr())) {
            if (it.is_regular_file()) {
                files.push_back(FilePath::fromStr(it.path().c_str()));
            }
        }
    } else {
        for (const auto& it : std::filesystem::directory_iterator(cstr())) {
            if (it.is_regular_file()) {
                files.push_back(FilePath::fromStr(it.path().c_str()));
            }
        }
    }
    return files;
}

String FilePath::read() const {
    FileReader reader(*this);
    String ret;
    while (!reader.eof()) {
        reader.readLine(ret);
        ret += '\n';
    }
    return ret;
}

std::vector<String> FilePath::readLines(bool includeEmptyLines) const {
    FileReader file = FileReader(*this);
    std::vector<String> lines;
    String line;
    while (!file.eof()) {
        file.readLine(line);
        if ((!includeEmptyLines) && (line.length() == 0)) { continue; }
        lines.push_back(line);
    }
    return lines;
}

std::vector<byte> FilePath::readBytes() const {
    std::ifstream file(cstr(), std::ios::ate | std::ios::binary);
    PGE_ASSERT(file.is_open(), "Could not open (file: \"" + str() + "\")");
    std::vector<byte> bytes;
    size_t vertSize = (size_t)file.tellg();
    bytes.resize(bytes.size() + vertSize);
    file.seekg(0);
    file.read((char*)bytes.data(), vertSize);
    return bytes;
}

const String& FilePath::str() const {
    return name;
}

const char* FilePath::cstr() const {
    return name.cstr();
}

bool FilePath::operator==(const FilePath& other) const {
    if (!isValid() || !other.isValid()) {
        return false;
    }
    return name == other.name;
}

bool FilePath::operator!=(const FilePath& other) const {
    if (!isValid() || !other.isValid()) {
        return true;
    }
    return name != other.name;
}

FilePath& FilePath::operator+=(const String& str) {
    PGE_ASSERT(valid, INVALID_STR);
    name += sanitizeFileSeperator(str);
    return *this;
}

const FilePath FilePath::operator+(const String& str) const {
    PGE_ASSERT(valid, INVALID_STR);
    return FilePath(name + sanitizeFileSeperator(str));
}

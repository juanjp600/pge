#include <PGE/File/FilePath.h>

#include <fstream>

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj_core.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

#include <PGE/Exception/Exception.h>
#include <PGE/File/TextReader.h>

using namespace PGE;

static const String INVALID_STR = "Tried using an invalid path";

static String sanitizeFileSeperator(const String& str) {
    return str.replace("\\", "/");
}

// Already sanitized.
static const String& getResourceStr() {
    static String resourceStr;
    if (resourceStr.isEmpty()) {
#if defined(__APPLE__) && defined(__OBJC__)
        NSBundle* bundle = [NSBundle mainBundle];
        resourceStr = String(bundle.resourcePath);
#else
        resourceStr = String(std::filesystem::current_path().generic_u8string().c_str());
#endif
        resourceStr = sanitizeFileSeperator(resourceStr);
        resourceStr += '/';
    }
    return resourceStr;
}

FilePath& FilePath::getDataPath() {
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
        String path(filePath);
        CoTaskMemFree(filePath);
        dataPath = FilePath::fromStr(path);
#endif
        dataPath = dataPath.makeDirectory();
    }
    return dataPath;
}

FilePath::FilePath(const String& str) noexcept {
    name = str;
}

FilePath::FilePath(const std::filesystem::path& path) {
    name = path.generic_u8string().c_str();
}

FilePath::FilePath() noexcept {
    name = "<n/a>";
    valid = false;
}

FilePath FilePath::fromStr(const String& str) {
    std::filesystem::path pth(str.c8str());
    String sanitizedStr = sanitizeFileSeperator(str);
    if (pth.is_absolute()) {
        return FilePath(sanitizedStr);
    } else {
        return FilePath(getResourceStr() + sanitizedStr);
    }
}

bool FilePath::isValid() const noexcept {
    return valid;
}

bool FilePath::isDirectory() const {
    PGE_ASSERT(valid, INVALID_STR);
    std::error_code err;
    bool isDir = std::filesystem::is_directory(str().c8str(), err);
    PGE_ASSERT(err.value() == 0, "Couldn't check if path is directory (dir: " + str() + "; err: " + err.message() + " (" + PGE::String::from(err.value()) + "))");
    return isDir;
}

FilePath FilePath::makeDirectory() const {
    PGE_ASSERT(valid, INVALID_STR);
    if (*str().charAt(name.length() - 1) != '/') {
        return *this + String("/");
    }
    return *this;
}

FilePath FilePath::getParentDirectory() const {
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

std::optional<String> FilePath::getRelativePath(const FilePath& other) const {
    PGE_ASSERT(valid, INVALID_STR);
    PGE_ASSERT(other.valid, INVALID_STR);
    auto ita = name.begin();
    auto itb = other.str().begin();
    while (ita != name.end() && itb != other.name.end()) {
        if (*ita != *itb) { return std::nullopt; }
        ita++; itb++;
    }
    if (ita == name.end()) {
        return other.name.substr(itb);
    } else {
        return name.substr(ita);
    }
}

String FilePath::getExtension() const {
    PGE_ASSERT(valid, INVALID_STR);
    String::Iterator startIndex = name.findLast(".");
    if (startIndex == name.end()) { return ""; }
    return name.substr(startIndex+1);
}

FilePath FilePath::trimExtension() const {
    PGE_ASSERT(valid, INVALID_STR);
    String::Iterator startIndex = name.findLast(".");
    if (startIndex == name.end()) { return *this; }
    return name.substr(name.begin(), startIndex);
}

bool FilePath::exists() const {
    PGE_ASSERT(valid, INVALID_STR);
    std::error_code err;
    bool exists = std::filesystem::exists(str().c8str(), err);
    PGE_ASSERT(err.value() == 0, "Couldn't check if directory exists (dir: " + str() + "; err: " + err.message() + " (" + PGE::String::from(err.value()) + "))");
    return exists;
}

u64 FilePath::getLastModifyTime() const {
    PGE_ASSERT(valid, INVALID_STR);
    std::error_code err;
    std::filesystem::file_time_type time = std::filesystem::last_write_time(str().c8str(), err);
    PGE_ASSERT(err.value() == 0, "Couldn't check directory modify time (dir: " + str() + "; err: " + err.message() + " (" + PGE::String::from(err.value()) + "))");
    return time.time_since_epoch().count();
}

bool FilePath::createDirectory() const {
    PGE_ASSERT(valid, INVALID_STR);
    std::error_code err;
    bool created = std::filesystem::create_directories(str().c8str(), err);
    PGE_ASSERT(err.value() == 0, "Couldn't create directory (dir: " + str() + "; err: " + err.message() + " (" + PGE::String::from(err.value()) + "))");
    return created;
}

std::vector<FilePath> FilePath::enumerateFolders() const {
    PGE_ASSERT(valid, INVALID_STR);
    std::vector<FilePath> folders;
    for (const auto& it : std::filesystem::directory_iterator(str().c8str())) {
        if (it.is_directory()) {
            folders.emplace_back(FilePath(it.path()));
        }
    }
    return folders;
}

std::vector<FilePath> FilePath::enumerateFiles(bool recursive) const {
    PGE_ASSERT(valid, INVALID_STR);
    std::vector<FilePath> files;
    if (recursive) {
        for (const auto& it : std::filesystem::recursive_directory_iterator(str().c8str())) {
            if (it.is_regular_file()) {
                files.emplace_back(FilePath(it.path()));
            }
        }
    } else {
        for (const auto& it : std::filesystem::directory_iterator(str().c8str())) {
            if (it.is_regular_file()) {
                files.emplace_back(FilePath(it.path()));
            }
        }
    }
    return files;
}

String FilePath::readText() const {
    String ret;
    readText(ret);
    return ret;
}

void FilePath::readText(String& text) const {
    // TextReader checks if the path is valid.
    TextReader reader(*this);
    text = String{ };
    while (!reader.endOfFile()) {
        reader.readLine(text);
        text += '\n';
    }
}

std::vector<String> FilePath::readLines(bool includeEmptyLines) const {
    // TextReader checks if the path is valid.
    TextReader reader(*this);
    std::vector<String> lines;
    String line;
    while (!reader.endOfFile()) {
        line = String();
        reader.readLine(line);
        if ((!includeEmptyLines) && line.isEmpty()) { continue; }
        lines.emplace_back(line);
    }
    return lines;
}

std::vector<byte> FilePath::readBytes() const {
    std::vector<byte> bytes;
    readBytes(bytes);
    return bytes;
}

void FilePath::readBytes(std::vector<byte>& bytes) const {
    PGE_ASSERT(valid, INVALID_STR);
    std::ifstream file(str().cstr(), std::ios::ate | std::ios::binary);
    PGE_ASSERT(file.is_open(), "Couldn't read bytes from file (file: \"" + str() + "\")");

    size_t size = (size_t)file.tellg();
    bytes.resize(size);
    file.seekg(0);
    file.read((char*)bytes.data(), size);
}

const String& FilePath::str() const {
    PGE_ASSERT(valid, INVALID_STR);
    return name;
}

bool FilePath::operator==(const FilePath& other) const noexcept {
    if (!isValid() || !other.isValid()) {
        return false;
    }
    return name == other.name;
}

void FilePath::operator+=(const String& str) {
    PGE_ASSERT(valid, INVALID_STR);
    name += sanitizeFileSeperator(str);
}

FilePath FilePath::operator+(const String& str) const {
    PGE_ASSERT(valid, INVALID_STR);
    return FilePath(name + sanitizeFileSeperator(str));
}

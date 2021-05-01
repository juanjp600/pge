#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#else
#include <filesystem>
#endif

#include <Misc/FilePath.h>
#include <Misc/FileUtil.h>
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

FilePath FilePath::makeDirectory() const {
    PGE_ASSERT(valid, INVALID_STR);
    if (str().charAt(length() - 1) != '/') {
        return *this + "/";
    }
    return *this;
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

String FilePath::getExtension() const {
    PGE_ASSERT(valid, INVALID_STR);
    int startIndex = name.findLast(".");
    if (startIndex < 0) { return ""; }
    return name.substr(startIndex+1);
}

int FilePath::byteLength() const {
    return name.byteLength();
}

int FilePath::length() const {
    return name.length();
}

bool FilePath::exists() const {
    PGE_ASSERT(valid, INVALID_STR);
    return FileUtil::exists(*this);
}

uint64_t FilePath::getHashCode() const {
    return name.getHashCode();
}

bool FilePath::equals(const FilePath& other) const {
    return name.equals(other.name) && valid == other.valid;
}

bool FilePath::isValid() const {
    return valid;
}

const FilePath PGE::operator+(const FilePath& a, const String& b) {
    return FilePath(a, b);
}

std::ostream& PGE::operator<<(std::ostream& os, const FilePath& fn) {
    return os.write(fn.cstr(), fn.byteLength());
}

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#else
#include <filesystem>
#endif

#include <Misc/FilePath.h>
#include <Misc/FileUtil.h>
#include <Exception/Exception.h>

using namespace PGE;

const String FilePath::INVALID_STR = "Tried using an invalid path";

FilePath::FilePath() {
    name = "<n/a>";
    valid = false;
}

String FilePath::getResourcePath() {
#if defined(__APPLE__) && defined(__OBJC__)
    NSBundle* bundle = [NSBundle mainBundle];
    return String(bundle.resourcePath, "/");
#endif
    return String(std::filesystem::current_path().c_str()) + "/";
}

FilePath FilePath::fromStr(const String& str) {
    FilePath fn;
    std::error_code err;
    fn.name = String(std::filesystem::absolute(str.cstr(), err).c_str()).replace("\\", "/");

    int errValue = err.value();
    __ASSERT(errValue == 0, "Failed to create path (str: " + str + "; ERRORCODE: " + String::fromInt(errValue) + ")");

    fn.valid = true;
    
    return fn;
}

FilePath::FilePath(const FilePath& a, const String& b) {
    __ASSERT(a.valid, INVALID_STR);
    name = a.str() + b;
    valid = true;
}

FilePath FilePath::validateAsDirectory() const {
    __ASSERT(valid, INVALID_STR);
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
    __ASSERT(valid, INVALID_STR);
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
    __ASSERT(valid, INVALID_STR);
    return FileUtil::exists(*this);
}

long long FilePath::getHashCode() const {
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

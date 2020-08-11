#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#else
#include <filesystem>
#endif

#include <Misc/FilePath.h>
#include <Misc/FileUtil.h>

using namespace PGE;

FilePath::FilePath() {
    name = "";
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
    if (str.charAt(1) == ':') {
        fn.name = str;
    } else {
        fn.name = getResourcePath().replace("\\", "/") + str;
    }
    
    return fn;
}

FilePath::FilePath(const FilePath& a, const String& b) {
    name = a.str() + b;
}

FilePath FilePath::validateAsDirectory() const {
    if (str().charAt(size() - 1) != '/') {
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

const wchar* FilePath::wstr() const {
    return name.wstr();
}

String FilePath::getExtension() const {
    int startIndex = name.findLast(".");
    if (startIndex < 0) { return ""; }
    return name.substr(startIndex+1);
}

int FilePath::size() const {
    return name.size();
}

bool FilePath::exists() const {
    return FileUtil::exists(*this);
}

FilePath& FilePath::operator=(const FilePath& other) {
    if (!equals(other)) {
        name = other.name;
    }
    
    return *this;
}

long long FilePath::getHashCode() const {
    return name.getHashCode();
}

bool FilePath::equals(const FilePath& other) const {
    return name.equals(other.name);
}

bool FilePath::isEmpty() const {
    return name.isEmpty();
}

const FilePath PGE::operator+(const FilePath& a, const String& b) {
    return FilePath(a, b);
}

std::ostream& PGE::operator<<(std::ostream& os, const FilePath& fn) {
    return os.write(fn.cstr(), fn.size());
}

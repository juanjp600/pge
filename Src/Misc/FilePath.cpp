#include <iostream>
#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#endif

#include <Misc/FilePath.h>
#include <Misc/FileUtil.h>

using namespace PGE;

FilePath::FilePath() {
    name = String("");
}

String FilePath::getResourcePath() {
#if defined(__APPLE__) && defined(__OBJC__)
    NSBundle* bundle = [NSBundle mainBundle];
    return String(bundle.resourcePath, "/");
#endif
    return String();
}

FilePath FilePath::fromStr(const String& str) {
    FilePath fn;
    fn.name = getResourcePath().replace("\\", "/") + str;
    
    return fn;
}

FilePath::FilePath(const FilePath& a, const String& b) {
    name = a.str() + b;
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

const FilePath operator+(const FilePath& a, const String& b) {
    return FilePath(a, b);
}

std::ostream& operator<<(std::ostream& os, const FilePath& fn) {
    return os.write(fn.cstr(), fn.size());
}

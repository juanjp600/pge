#include <iostream>

#include <Misc/FilePath.h>
#include <Misc/FileUtil.h>

using namespace PGE;

FilePath::FilePath() {
    name = String("");
}

FilePath FilePath::fromStr(const String& str) {
    FilePath fn;
    fn.name = str.resourcePath().replace("\\", "/");
    
    return fn;
}

FilePath::FilePath(const FilePath& a, const String& b) {
    name = a.str() + b;
}

const String FilePath::str() const {
    return name;
}

const char* FilePath::cstr() const {
    return name.cstr();
}

const wchar* FilePath::wstr() const {
    return name.wstr();
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

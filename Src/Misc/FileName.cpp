#include <iostream>

#include <Misc/FileName.h>
#include <Misc/FileUtil.h>

using namespace PGE;

FileName::FileName() {
    name = String("");
}

FileName FileName::fromStr(const String& str) {
    FileName fn;
    fn.name = str.resourcePath();
    
    return fn;
}

FileName::FileName(const FileName& a, const String& b) {
    name = a.str() + b;
}

const String FileName::str() const {
    return name;
}

const char* FileName::cstr() const {
    return name.cstr();
}

const wchar* FileName::wstr() const {
    return name.wstr();
}

int FileName::size() const {
    return name.size();
}

bool FileName::exists() const {
    return FileUtil::exists(str());
}

FileName& FileName::operator=(const FileName& other) {
    if (!equals(other)) {
        name = other.name;
    }
    
    return *this;
}

long long FileName::getHashCode() const {
    return name.getHashCode();
}

bool FileName::equals(const FileName& other) const {
    return name.equals(other.name);
}

bool FileName::isEmpty() const {
    return name.isEmpty();
}

const FileName operator+(const FileName& a, const String& b) {
    return FileName(a, b);
}

std::ostream& operator<<(std::ostream& os, const FileName& fn) {
    return os.write(fn.cstr(), fn.size());
}

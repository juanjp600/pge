#include <iostream>

#include <Misc/FileName.h>

using namespace PGE;

FileName::FileName() {
    name = String("");
}

FileName::FileName(const String& str) {
    name = str.resourcePath();
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

FileName& FileName::operator=(const String& other) {
    if (!name.equals(other)) {
        name = other.resourcePath();
    }

    return *this;
}

long long FileName::getHashCode() const {
    return name.getHashCode();
}

bool FileName::equals(const String& other) const {
    return name.equals(other);
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

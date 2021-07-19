#include <PGE/String/String.h>
#include "UnicodeHelper.h"
#include "Unicode.h"

#include <iostream>
#include <queue>
#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#endif

#include <PGE/Exception/Exception.h>

using namespace PGE;

//
// Iterator
//

String::Iterator::Iterator() {
    ref = nullptr;
    index = -1;
    charIndex = -1;
}

String::Iterator::Iterator(const String& str, int byteIndex, int chIndex) {
    ref = &str;
    index = byteIndex;
    charIndex = chIndex;
}

void String::Iterator::increment() {
    index += Unicode::measureCodepoint(ref->cstr()[index]);
    charIndex++;
    // We reached the end and get the str length for free.
    if (index == ref->byteLength()) {
        ref->data->_strLength = charIndex;
    }
    _ch = L'\uFFFF';
}

void String::Iterator::decrement() {
    charIndex--;
    index--;
    // First bit 0 means it's a single byte codepoint.
    if ((ref->cstr()[index] & 0b1000'0000) != 0) {
        // Otherwise we go backwards until we find the first byte of the character.
        while ((ref->cstr()[index] & 0b1100'0000) == 0b1000'0000) {
            index--;
        }
    }
    _ch = L'\uFFFF';
}

String::Iterator& String::Iterator::operator++() {
    PGE_ASSERT(index < ref->byteLength(), "Can't increment iterator past string end");
    increment();
    return *this;
}

String::Iterator& String::Iterator::operator--() {
    PGE_ASSERT(index > 0, "Can't decrement iterator prior to string beginning");
    decrement();
    return *this;
}

const String::Iterator String::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

const String::Iterator String::Iterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

const String::Iterator String::Iterator::operator+(int steps) const {
    if (steps < 0) { return *this - (-steps); }
    String::Iterator ret = *this;
    for (int i = 0; i < steps; i++) {
        ret++;
    }
    return ret;
}

const String::Iterator String::Iterator::operator-(int steps) const {
    if (steps < 0) { return *this + (-steps); }
    String::Iterator ret = *this;
    for (int i = 0; i < steps; i++) {
        ret--;
    }
    return ret;
}

String::Iterator& String::Iterator::operator+=(int steps) {
    if (steps < 0) { return *this -= (-steps); }
    for (int i = 0; i < steps; i++) {
        (*this)++;
    }
    return *this;
}

String::Iterator& String::Iterator::operator-=(int steps) {
    if (steps < 0) { return *this += (-steps); }
    for (int i = 0; i < steps; i++) {
        (*this)--;
    }
    return *this;
}

int String::Iterator::operator-(const String::Iterator& other) const {
    return getPosition() - other.getPosition();
}

char16 String::Iterator::operator*() const {
    PGE_ASSERT(index >= 0 && index < ref->byteLength(), "Tried dereferencing invalid iterator");
    if (_ch == L'\uFFFF') {
        _ch = Unicode::utf8ToWChar(ref->cstr() + index);
    }
    return _ch;
}

bool String::Iterator::operator<(const Iterator& other) const {
    return ref->chs == other.ref->chs && index < other.index;
}

bool String::Iterator::operator>(const Iterator& other) const {
    return ref->chs == other.ref->chs && index > other.index;
}

bool String::Iterator::operator<=(const Iterator& other) const {
    return ref->chs == other.ref->chs && index <= other.index;
}

bool String::Iterator::operator>=(const Iterator& other) const {
    return ref->chs == other.ref->chs && index >= other.index;
}

bool String::Iterator::operator==(const Iterator& other) const {
    return ref->chs == other.ref->chs && index == other.index;
}

bool String::Iterator::operator!=(const Iterator& other) const {
    return ref->chs != other.ref->chs || index != other.index;
}

int String::Iterator::getBytePosition() const {
    return index;
}

int String::Iterator::getPosition() const {
    if (charIndex < 0) {
        charIndex = 0;
        for (int i = 0; i < index; i += Unicode::measureCodepoint(ref->cstr()[i])) {
            charIndex++;
        }
    }
    return charIndex;
}

const String::Iterator String::Iterator::begin(const String& str) {
    return Iterator(str, 0, 0);
}

const String::Iterator String::Iterator::end(const String& str) {
    // We need byteLength for functionality, but length is optional.
    return Iterator(str, str.byteLength(), str.data->_strLength);
}

const String::Iterator String::begin() const {
    return Iterator::begin(*this);
}

const String::Iterator String::end() const {
    return Iterator::end(*this);
}

String::ReverseIterator& String::ReverseIterator::operator++() {
    PGE_ASSERT(index >= 0, "Can't decrement iterator prior to string beginning");
    decrement();
    return *this;
}

String::ReverseIterator& String::ReverseIterator::operator--() {
    PGE_ASSERT(index < ref->byteLength() - 1, "Can't increment iterator past string end");
    increment();
    return *this;
}

const String::ReverseIterator String::ReverseIterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

const String::ReverseIterator String::ReverseIterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

const String::ReverseIterator String::ReverseIterator::rbegin(const String& str) {
    return end(str) - 1;
}

const String::ReverseIterator String::ReverseIterator::rend(const String& str) {
    return ReverseIterator(str, -1, -1);
}

const String::ReverseIterator String::rbegin() const {
    return String::ReverseIterator::rbegin(*this);
}

const String::ReverseIterator String::rend() const {
    return String::ReverseIterator::rend(*this);
}

//

void String::copy(String& dst, const String& src) {
    dst.internalData = src.internalData;
    if (src.data->cCapacity == SHORT_STR_CAPACITY) {
        Unique& u = std::get<Unique>(dst.internalData);
        dst.chs = u.chs;
        dst.data = &u.data;
    } else {
        dst.chs = src.chs;
        dst.data = src.data;
    }
}

String::String() {
    // Manual metadata:
    data->strByteLength = 0;
    data->_strLength = 0;
    data->_hashCode = FNV_SEED;
    data->_hashCodeEvaluted = true;
    cstrNoConst()[0] = '\0';
}

String::String(const String& other) {
    copy(*this, other);
}

String::String(const std::string& cppstr) {
    int len = (int)cppstr.size();
    reallocate(len);
    data->strByteLength = len;
    memcpy(cstrNoConst(), cppstr.c_str(), len + 1);
}

String::String(const char16* wstri) {
    wCharToUtf8Str(wstri);
}

void String::wCharToUtf8Str(const char16* wbuffer) {
    // Determine the capacity of the cbuffer by measuring the number of bytes required for each codepoint.
    int newCap = 0;
    for (int i = 0; wbuffer[i] != L'\0'; i++) {
        newCap += Unicode::wCharToUtf8(wbuffer[i], nullptr);
    }
    reallocate(newCap);

    // Convert all the wchars to codepoints.
    char* buf = cstrNoConst();
    int cIndex = 0;
    // We get _strLength "for free" here.
    for (data->_strLength = 0; wbuffer[data->_strLength] != L'\0'; data->_strLength++) {
        cIndex += Unicode::wCharToUtf8(wbuffer[data->_strLength], &buf[cIndex]);
    }
    buf[newCap] = '\0';
    data->strByteLength = newCap;
}

#if defined(__APPLE__) && defined(__OBJC__)
String::String(const NSString* nsstr) {
    const char* cPath = [nsstr cStringUsingEncoding: NSUTF8StringEncoding];
    int len = (int)strlen(cPath);
    reallocate(len);
    strByteLength = len;
    memcpy(cstrNoConst(), cPath, len + 1);
}
#endif

String::String(char c) {
    char* buf = cstrNoConst();
    if (c < 0) {
        reallocate(2);
        data->strByteLength = Unicode::wCharToUtf8((char16)(unsigned char)c, buf);
        buf[data->strByteLength] = '\0';
    } else {
        reallocate(1);
        data->strByteLength = 1;
        buf[0] = c;
        buf[1] = '\0';
    }
    data->_strLength = 1;
}

String::String(char16 w) {
    reallocate(4);
    char* buf = cstrNoConst();
    data->strByteLength = Unicode::wCharToUtf8(w, buf);
    data->_strLength = 1;
    buf[data->strByteLength] = '\0';
}

//
// Private constructors.
//

String::String(int size) {
    reallocate(size);
}

// Byte substr.
String::String(const String& other, int from, int cnt) {
    reallocate(cnt);
    data->strByteLength = cnt;
    char* buf = cstrNoConst();
    memcpy(buf, other.cstr() + from, cnt);
    buf[cnt] = '\0';
}

template <typename T>
const String String::format(T t, const String& format) {
    int size = snprintf(nullptr, 0, format.cstr(), t);
    String ret(size);
    // From my (limited) research these should be safe to use, even with UTF-8 strings, as 0x25 never appears in any UTF-8 character.
    sprintf(ret.cstrNoConst(), format.cstr(), t);
    ret.data->strByteLength = size;
    return ret;
}

template const String String::format<u8>(u8 t, const PGE::String& format);
template const String String::format<u16>(u16 t, const PGE::String& format);
template const String String::format<u32>(u32 t, const PGE::String& format);
template const String String::format<u64>(u64 t, const PGE::String& format);
template const String String::format<i8>(i8 t, const PGE::String& format);
template const String String::format<i16>(i16 t, const PGE::String& format);
template const String String::format<i32>(i32 t, const PGE::String& format);
template const String String::format<i64>(i64 t, const PGE::String& format);
template const String String::format<float>(float t, const PGE::String& format);
template const String String::format<double>(double t, const PGE::String& format);
template const String String::format<long double>(long double t, const PGE::String& format);

const String String::fromInt(int i) {
    // "-2147483648" has 11 characters.
    String ret(11);
    ret.data->_strLength = sprintf(ret.cstrNoConst(), "%d", i);
    ret.data->strByteLength = ret.data->_strLength;
    return ret;
}

const String String::fromFloat(float f) {
    // Scientific notation to severly limit maximum output length.
    // sign + 6 * digits + point + e + expsign + 2 * expdigits
    String ret(12);
    ret.data->_strLength = sprintf(ret.cstrNoConst(), "%g", f);
    ret.data->strByteLength = ret.data->_strLength;
    return ret;
}

String& String::operator=(const String& other) {
    copy(*this, other);
    return *this;
}

String& String::operator+=(const String& other) {
    int oldByteSize = byteLength();
    int newSize = oldByteSize + other.byteLength();
    reallocate(newSize, true);
    char* buf = cstrNoConst();
    memcpy(buf + oldByteSize, other.cstr(), other.byteLength() + 1);
    data->strByteLength = newSize;
    if (data->_strLength >= 0 && other.data->_strLength >= 0) {
        data->_strLength += other.length();
    }
    return *this;
}

String& String::operator+=(char16 ch) {
    int aLen = byteLength();
    reallocate(aLen + 4, true);
    char* buf = cstrNoConst();
    int actualSize = aLen + Unicode::wCharToUtf8(ch, buf + aLen);
    buf[actualSize] = '\0';
    data->strByteLength = actualSize;
    if (data->_strLength >= 0) {
        data->_strLength++;
    }
    return *this;
}

const String PGE::operator+(const String& a, const String& b) {
    int aLen = a.byteLength();
    int bLen = b.byteLength();
    String ret(aLen + bLen);
    char* buf = ret.cstrNoConst();
    memcpy(buf, a.cstr(), aLen);
    memcpy(buf + aLen, b.cstr(), bLen + 1);
    ret.data->strByteLength = aLen + bLen;
    if (a.data->_strLength >= 0 && b.data->_strLength >= 0) {
        ret.data->_strLength = a.length() + b.length();
    }
    return ret;
}

const String PGE::operator+(const char* a, const String& b) {
    int aLen = (int)strlen(a);
    int bLen = b.byteLength();
    String ret(aLen + bLen);
    char* buf = ret.cstrNoConst();
    memcpy(buf, a, aLen);
    memcpy(buf + aLen, b.cstr(), bLen + 1);
    ret.data->strByteLength = aLen + bLen;
    return ret;
}

const String PGE::operator+(const String& a, const char* b) {
    int aLen = a.byteLength();
    int bLen = (int)strlen(b);
    String ret(aLen + bLen);
    char* buf = ret.cstrNoConst();
    memcpy(buf, a.cstr(), aLen);
    memcpy(buf + aLen, b, bLen + 1);
    ret.data->strByteLength = aLen + bLen;
    return ret;
}

const String PGE::operator+(const String& a, char16 b) {
    int aLen = a.byteLength();
    String ret(aLen + 4);
    char* buf = ret.cstrNoConst();
    memcpy(buf, a.cstr(), aLen);
    int actualSize = aLen + Unicode::wCharToUtf8(b, buf + aLen);
    buf[actualSize] = '\0';
    ret.data->strByteLength = actualSize;
    if (a.data->_strLength >= 0) {
        ret.data->_strLength = a.length() + 1;
    }
    return ret;
}

const String PGE::operator+(char16 a, const String& b) {
    int bLen = b.byteLength();
    String ret(4 + bLen);
    char* buf = ret.cstrNoConst();
    int writtenChars = Unicode::wCharToUtf8(a, buf);
    memcpy(buf + writtenChars, b.cstr(), bLen);
    int actualSize = writtenChars + bLen;
    buf[actualSize] = '\0';
    ret.data->strByteLength = actualSize;
    if (b.data->_strLength >= 0) {
        ret.data->_strLength = b.length() + 1;
    }
    return ret;
}

bool PGE::operator==(const String& a, const String& b) {
    return a.equals(b);
}

bool PGE::operator!=(const String& a, const String& b) {
    return !a.equals(b);
}

std::ostream& PGE::operator<<(std::ostream& os, const String& s) {
    return os.write(s.cstr(), s.byteLength());
}

// ASCII, fuck you.
std::istream& PGE::operator>>(std::istream& is, String& s) {
    // See xstring for reference.

    int ch;
    while ((ch = is.rdbuf()->sbumpc()) != EOF && ch != '\r' && ch != '\n') {
        s += (char)ch;
    }
    if (ch == EOF) {
        is.setstate(std::ios::eofbit | std::ios::failbit);
    }
    return is;
}

u64 String::getHashCode() const {
    if (!data->_hashCodeEvaluted) {
        // FNV-1a
        // Public domain
        byte* buf = (byte*)cstr();
        data->_hashCode = FNV_SEED;
        for (int i = 0; buf[i] != '\0'; i++) {
            data->_hashCode ^= buf[i];
            data->_hashCode *= 0x00000100000001b3u;
        }
        data->_hashCodeEvaluted = true;
    }
    return data->_hashCode;
}

bool String::equals(const String& other) const {
    if (chs == other.chs) { return true; }
    if (byteLength() != other.byteLength()) { return false; }
    if (data->_strLength >= 0 && other.data->_strLength >= 0 && length() != other.length()) { return false; }
    if (data->_hashCodeEvaluted && other.data->_hashCodeEvaluted) { return getHashCode() == other.getHashCode(); }
    return strcmp(cstr(), other.cstr()) == 0;
}

static void fold(const char*& buf, std::queue<char16>& queue) {
    if (queue.empty() && *buf != '\0') {
        int codepoint = Unicode::measureCodepoint(*buf);
        char16 ch = Unicode::utf8ToWChar(buf, codepoint);
        Unicode::fold(queue, ch);
        buf += codepoint;
    }
}

bool String::equalsIgnoreCase(const String& other) const {
    if (chs == other.chs) { return true; }
    if (data->_hashCodeEvaluted && other.data->_hashCodeEvaluted && getHashCode() == other.getHashCode()) { return true; }

    const char* buf[2] = { cstr(), other.cstr() };
    std::queue<char16> queue[2];

    // Feed first char.
    for (int i = 0; i < 2; i++) {
        fold(buf[i], queue[i]);
    }

    while (!queue[0].empty() && !queue[1].empty()) {
        if (queue[0].front() == queue[1].front()) {
            // Continue, are we done yet?
            queue[0].pop();
            queue[1].pop();
        } else {
            return false;
        }

        // Try refilling.
        for (int i = 0; i < 2; i++) {
            fold(buf[i], queue[i]);
        }
    }

    // If the strings are really equal, then both have the null char now.
    return *buf[0] == *buf[1];
}

bool String::isEmpty() const {
    return chs[0] == '\0';
}

void String::reallocate(int size, bool copyOldChs) {
    // Accounting for the terminating byte.
    size++;

    // Initialized with String literal.
    if (data->cCapacity == 0) {
        if (size <= SHORT_STR_CAPACITY) {
            Unique& u = internalData.emplace<Unique>();
            if (copyOldChs) {
                memcpy(u.chs, chs, data->strByteLength);
            }
            u.data = *data;
            u.data.cCapacity = SHORT_STR_CAPACITY;
            chs = u.chs;
            data = &u.data;
            return;
        }
    } else {
        if (size <= data->cCapacity) {
            if (data->cCapacity == SHORT_STR_CAPACITY) {
                return;
            }
            std::shared_ptr<Shared>& s = std::get<std::shared_ptr<Shared>>(internalData);
            if (s.use_count() == 1) {
                chs = s->chs.get();
                data = &s->data;
                return;
            }
        }
    }

    int targetCapacity = data->cCapacity;
    while (targetCapacity < size) { targetCapacity <<= 1; }

    std::unique_ptr<char[]> newChs = std::make_unique<char[]>(targetCapacity);
    if (copyOldChs) {
        memcpy(newChs.get(), cstr(), data->strByteLength);
    }

    internalData = std::make_shared<Shared>();
    std::shared_ptr<Shared>& s = std::get<std::shared_ptr<Shared>>(internalData);
    s->chs = std::move(newChs);

    chs = s->chs.get();
    data = &s->data;
    data->cCapacity = targetCapacity;
}

const char* String::cstr() const {
    return chs;
}

char* String::cstrNoConst() {
    return chs;
}

std::vector<char16> String::wstr() const {
    std::vector<char16> chars;
    if (data->_strLength >= 0) {
        chars.reserve(data->_strLength);
    }
    // Convert all the codepoints to wchars.
    for (Iterator it = begin(); it != end(); it++) {
        chars.push_back(*it);
    }
    chars.push_back(L'\0');
    return chars;
}

int String::toInt(bool& success) const {
    try {
        success = true;
        return std::stoi(cstr());
    } catch (const std::exception&) {
        success = false;
        return 0;
    }
}

float String::toFloat(bool& success) const {
    try {
        success = true;
        return std::stof(cstr());
    } catch (const std::exception&) {
        success = false;
        return 0.f;
    }
}

int String::toInt() const {
    bool discard;
    return toInt(discard);
}

float String::toFloat() const {
    bool discard;
    return toFloat(discard);
}

int String::length() const {
    if (data->_strLength < 0) {
        const char* buf = cstr();
        data->_strLength = 0;
        for (int i = 0; buf[i] != '\0'; i += Unicode::measureCodepoint(buf[i])) {
            data->_strLength++;
        }
    }
    return data->_strLength;
}

int String::byteLength() const {
    PGE_ASSERT(data->strByteLength >= 0, "String byte length must always be valid");
    return data->strByteLength;
}

const String::Iterator String::findFirst(const String& fnd, int from) const {
    return findFirst(fnd, begin() + from);
}

static const String EMPTY_FIND = "Find string can't be empty";

const String::Iterator String::findFirst(const String& fnd, const Iterator& from) const {
    PGE_ASSERT(!fnd.isEmpty(), EMPTY_FIND);
    for (auto it = from; it != end(); it++) {
        if (memcmp(fnd.cstr(), cstr() + it.getBytePosition(), fnd.byteLength()) == 0) { return it; }
    }
    return end();
}

const String::ReverseIterator String::findLast(const String& fnd, int fromEnd) const {
    return findLast(fnd, rbegin() + fromEnd);
}

const String::ReverseIterator String::findLast(const String& fnd, const ReverseIterator& from) const {
    PGE_ASSERT(!fnd.isEmpty(), EMPTY_FIND);
    for (auto it = from; it != rend(); it++) {
        if (memcmp(fnd.cstr(), cstr() + it.getBytePosition(), fnd.byteLength()) == 0) { return it; }
    }
    return rend();
}

const String String::substr(int start) const {
    return substr(begin() + start);
}

const String String::substr(int start, int cnt) const {
    Iterator from = begin() + start;
    return substr(from, from + cnt);
}

const String String::substr(const Iterator& start) const {
    return substr(start, end());
}

const String String::substr(const Iterator& start, const Iterator& to) const {
    PGE_ASSERT(start.getBytePosition() <= to.getBytePosition(),
        "Start iterator can't come after to iterator (start: " + fromInt(start.getBytePosition()) + "; to: " + fromInt(to.getBytePosition()) + "; str: " + *this + ")");
    PGE_ASSERT(to.getBytePosition() <= end().getBytePosition(),
        "To iterator can't come after end iterator (to: " + fromInt(to.getBytePosition()) + "; end: " + fromInt(end().getBytePosition()) + "; str: " + *this + ")");

    int newSize = to.getBytePosition() - start.getBytePosition();
    String retVal(newSize);
    retVal.data->strByteLength = newSize;
    // Due to not being friends with Iterators, we just bite the bullet here and hope for the best.
    retVal.data->_strLength = to.getPosition() - start.getPosition();
    char* retBuf = retVal.cstrNoConst();
    memcpy(retBuf, cstr() + start.getBytePosition(), newSize);
    retBuf[newSize] = '\0';
    return retVal;
}

const String::Iterator String::charAt(int pos) const {
    Iterator it;
    for (it = begin(); it != end() && it.getPosition() != pos; it++);
    return it;
}

const String String::replace(const String& fnd, const String& rplace) const {
    PGE_ASSERT(fnd.byteLength() != 0, "Find string can't be empty");

    const char* fndStr = fnd.cstr();
    const char* rplaceStr = rplace.cstr();
    const char* thisStr = cstr();

    std::vector<int> foundPositions;
    for (int i = 0; i <= byteLength() - fnd.byteLength();) {
        if (memcmp(fndStr, thisStr + i, fnd.byteLength()) == 0) {
            foundPositions.push_back(i);
            i += fnd.byteLength();
        } else {
            i++;
        }
    }
    
    int newSize = byteLength() + (int)foundPositions.size() * (rplace.byteLength() - fnd.byteLength());
    String retVal(newSize);
    retVal.data->strByteLength = newSize;

    char* retBuf = retVal.cstrNoConst();
    int retPos = 0;
    int thisPos = 0;
    for (int pos : foundPositions) {
        int thisLen = pos - thisPos;
        memcpy(retBuf + retPos, thisStr + thisPos, thisLen);
        retPos += thisLen;
        memcpy(retBuf + retPos, rplaceStr, rplace.byteLength());
        retPos += rplace.byteLength();
        thisPos = pos + fnd.byteLength();
    }
    // Append the rest of the string, including terminating byte.
    memcpy(retBuf + retPos, thisStr + thisPos, byteLength() - thisPos + 1);
   
    // If the string that is being operated on already has had its length calculated, we assume it to be worth it to pre-calculate the new string's length.
    if (data->_strLength >= 0) {
        retVal.data->_strLength = data->_strLength + (int)foundPositions.size() * (rplace.length() - fnd.length());
    }
    return retVal;
}

// Only has to deal with data.
void String::initLiteral(int litSize) {
    static std::unordered_map<const char*, Data> litData;
    if (const auto& it = litData.find(chs); it != litData.end()) {
        data = &it->second;
    } else {
        data = &litData.emplace(chs, Data()).first->second;
        data->cCapacity = 0;
        data->strByteLength = litSize - 1;
    }
}

// TODO: Funny special cases!
const String String::performCaseConversion(const std::function<void (String&, char16)>& func) const {
    String ret(byteLength());
    ret.data->strByteLength = 0;
    ret.data->_strLength = 0;
    for (char16 ch : *this) {
        func(ret, ch);
    }
    return ret;
}

const String String::toUpper() const {
    return performCaseConversion(Unicode::up);
}

const String String::toLower() const {
    return performCaseConversion(Unicode::down);
}

const String String::trim() const {
    if (byteLength() == 0) { return *this; }

    Iterator leading = begin();
    while (leading != end() && isspace(*leading)) {
        leading++;
    }
    if (leading == end()) {
        return "";
    }

    ReverseIterator trailing = rbegin();
    while (isspace(*trailing)) { trailing++; }

    String ret(*this, leading.getBytePosition(), trailing.getBytePosition() - leading.getBytePosition() + 1);
    return ret;
}

const String String::reverse() const {
    int len = byteLength();
    String ret(len);
    char* buf = ret.cstrNoConst();
    buf[len] = '\0';
    buf += len;
    for (int i = 0; i < len;) {
        int codepoint = Unicode::measureCodepoint(cstr()[i]);
        buf -= codepoint;
        memcpy(buf, cstr() + i, codepoint);
        i += codepoint;
    }
    ret.data->strByteLength = len;
    ret.data->_strLength = data->_strLength;
    return ret;
}

const String String::multiply(int count, const String& separator) const {
    int curLength = byteLength();
    int sepLength = separator.byteLength();
    int newLength = curLength * count + sepLength * (count - 1);
    String ret(newLength);
    char* buf = ret.cstrNoConst();
    buf[newLength] = '\0';
    for (int i = 0; i < count; i++) {
        if (i != 0) {
            memcpy(buf, separator.cstr(), sepLength);
            buf += sepLength;
        }
        memcpy(buf, cstr(), curLength);
        buf += curLength;
    }
    ret.data->strByteLength = newLength;
    if (data->_strLength >= 0) { ret.data->_strLength = length() * count + separator.length() * (count - 1); }
    return ret;
}

std::vector<String> String::split(const String& needleStr, bool removeEmptyEntries) const {
    std::vector<String> split;
    const char* haystack = cstr();
    const char* needle = needleStr.cstr();
    int codepoint;
    int cut = 0;
    for (int i = 0; i <= byteLength() - needleStr.byteLength(); i += codepoint) {
        codepoint = Unicode::measureCodepoint(haystack[i]);
        if (memcmp(haystack + i, needle, codepoint) == 0) {
            int addSize = i - cut;
            if (!removeEmptyEntries || addSize != 0) {
                split.push_back(String(*this, cut, addSize));
            }
            cut = i + needleStr.byteLength();
        }
    }
    // Add the rest of the string to the vector.
    int endAddSize = byteLength() - cut;
    if (endAddSize != 0) {
        split.push_back(String(*this, cut, endAddSize));
    }
    return split;
}

const String String::join(const std::vector<String>& vect, const String& separator) {
    if (vect.size() == 0) {
        return String();
    }

    String retVal = vect[0];
    for (int i = 1; i < (int)vect.size(); i++) {
        retVal = retVal + separator + vect[i];
    }

    return retVal;
}

const std::cmatch String::regexMatch(const std::regex& pattern) const {
    const char* s = cstr();
    std::cmatch m;
    std::regex_search(s, m, pattern);

    return m;
}

/* TODO: Improve this if we get MacOS support back. Non-fixed return value size and possibly propagating more metadata.
String String::unHex() const {
    int isUnhexing = 0;

    const char* buf = cstr();

    String ret(64);
    char* retBuf = ret.cstrNoConst();
    int resultSize = 0;

    int tempAscVal = 0;
    int codepoint;
    for (int i=0;i<byteLength();i+=codepoint) {
        codepoint = measureCodepoint(buf[i]);
        if (codepoint == 1) {
            if (isUnhexing == 0) {
                if (buf[i] == '%') {
                    isUnhexing = 1;
                } else {
                    retBuf[resultSize] = buf[i];
                    resultSize++;
                }
            } else {
                int toDec = 0;
                if (buf[i] >= '0' && buf[i] <= '9') {
                    toDec = buf[i] - '0';
                } else if (buf[i] >= 'a' && buf[i] <= 'f') {
                    toDec = buf[i] + 10 - 'a';
                } else if (buf[i] >= 'A' && buf[i] <= 'F') {
                    toDec = buf[i] + 10 - 'A';
                }
                if (isUnhexing == 1) {
                    tempAscVal = toDec << 4;
                    isUnhexing = 2;
                } else if (isUnhexing == 2) {
                    tempAscVal |= toDec;
                    retBuf[resultSize] = tempAscVal;
                    resultSize++;
                    isUnhexing = 0;
                }
            }
        } else {
            for (int j = 0; j < codepoint; j++) {
                retBuf[i + j] = buf[i + j];
            }
        }
    }
    retBuf[resultSize] = '\0';
    ret.strByteLength = resultSize;
    return ret;
}*/

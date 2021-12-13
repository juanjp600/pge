#include <PGE/String/String.h>
#include <PGE/String/Unicode.h>
#include "UnicodeInternal.h"
#include "UnicodeHelper.h"

#include <limits>
#include <iostream>
#include <queue>
#include <mutex>
#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#endif

#include <PGE/Exception/Exception.h>
#include <PGE/Math/Hasher.h>

using namespace PGE;

//
// Iterator
//

void String::BasicIterator::increment() {
    if (index < 0) { *this = ref->begin(); return; }
    index += Unicode::measureCodepoint(ref->cstr()[index]);
    charIndex++;
    // We reached the end and get the str length for free.
    if (index == ref->byteLength()) {
        ref->getData()->_strLength = charIndex;
    }
    _ch = L'\uFFFF';
}

void String::BasicIterator::decrement() {
    charIndex--;
    index--;
    if (index < 0) { return; }
    // First bit 0 means it's a single byte codepoint.
    if ((ref->cstr()[index] & 0b1000'0000) != 0) {
        // Otherwise we go backwards until we find the first byte of the character.
        while ((ref->cstr()[index] & 0b1100'0000) == 0b1000'0000) {
            index--;
        }
    }
    _ch = L'\uFFFF';
}

int String::BasicIterator::operator-(const String::BasicIterator& other) const {
    return getPosition() - other.getPosition();
}

char16 String::BasicIterator::operator*() const {
    asrt(index >= 0 && index < ref->byteLength(), "Tried dereferencing invalid iterator");
    if (_ch == L'\uFFFF') {
        _ch = Unicode::utf8ToWChar(ref->cstr() + index);
    }
    return _ch;
}

bool String::BasicIterator::operator==(const BasicIterator& other) const {
    return ref->getChars() == other.ref->getChars() && index == other.index;
}

int String::BasicIterator::getBytePosition() const {
    return index;
}

int String::BasicIterator::getPosition() const {
    if (charIndex < 0) {
        charIndex = 0;
        for (int i = 0; i < index; i += Unicode::measureCodepoint(ref->cstr()[i])) {
            charIndex++;
        }
    }
    return charIndex;
}

String::Iterator& String::Iterator::operator++() {
    asrt(index < ref->byteLength(), "Tried incrementing end iterator");
    increment();
    return *this;
}

String::Iterator& String::Iterator::operator--() {
    asrt(index > 0, "Tried decrementing begin iterator");
    decrement();
    return *this;
}

String::ReverseIterator& String::ReverseIterator::operator++() {
    asrt(index >= 0, "Tried decrementing end reverse iterator");
    decrement();
    return *this;
}

String::ReverseIterator& String::ReverseIterator::operator--() {
    asrt(index < String::Iterator::end(*ref).index, "Tried incrementing begin reverse iterator");
    increment();
    return *this;
}

const inline String INVALID_ITERATOR = "Tried reversing invalid iterator";

void String::Iterator::validate() {
    asrt(index >= 0, INVALID_ITERATOR);
}

void String::ReverseIterator::validate() {
    asrt(index < ref->byteLength(), INVALID_ITERATOR);
}

const String::Iterator String::Iterator::begin(const String& str) {
    return String::Iterator(str, 0, 0);
}

template <> const String::Iterator String::Iterator::end(const String& str) {
    return String::Iterator(str, str.byteLength(), str.getData()->_strLength);
}

const String::ReverseIterator String::ReverseIterator::begin(const String& str) {
    return str.isEmpty() ? end(str) : (ReverseIterator)(Iterator::end(str) - 1);
}

template <> const String::ReverseIterator String::ReverseIterator::end(const String& str) {
    return String::ReverseIterator(str, -1, -1);
}

const String::Iterator String::begin() const {
    return Iterator::begin(*this);
}

const String::Iterator String::end() const {
    return Iterator::end(*this);
}

const String::ReverseIterator String::rbegin() const {
    return String::ReverseIterator::begin(*this);
}

const String::ReverseIterator String::rend() const {
    return String::ReverseIterator::end(*this);
}

//

void String::copy(String& dst, const String& src) {
    dst.internalData = src.internalData;
}

String::String() {
    // Manual metadata:
    Data* data = getData();
    data->strByteLength = 0;
    data->_strLength = 0;
    data->_hashCode = Hasher().getHash();
    cstrNoConst()[0] = '\0';
}

String::String(const String& other) {
    copy(*this, other);
}

String::String(const char8_t* cstr)
    : String((const char*)cstr) { }

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
    Data* data = getData();
    // We get _strLength "for free" here.
    for (data->_strLength = 0; wbuffer[data->_strLength] != L'\0'; data->_strLength++) {
        cIndex += Unicode::wCharToUtf8(wbuffer[data->_strLength], &buf[cIndex]);
    }
    buf[newCap] = '\0';
    data->strByteLength = newCap;
}

String::String(const std::string& cppstr) {
    int len = (int)cppstr.size();
    reallocate(len);
    getData()->strByteLength = len;
    memcpy(cstrNoConst(), cppstr.c_str(), len + 1);
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
    Data* data = getData();
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

String::String(char8_t c)
    : String((char)c) { }

String::String(char16 w) {
    reallocate(4);
    Data* data = getData();
    char* buf = cstrNoConst();
    data->strByteLength = Unicode::wCharToUtf8(w, buf);
    data->_strLength = 1;
    buf[data->strByteLength] = '\0';
}

String::String(const String& a, const String& b) {
    int aLen = a.byteLength();
    int bLen = b.byteLength();
    reallocate(aLen + bLen);
    char* buf = cstrNoConst();
    memcpy(buf, a.cstr(), aLen);
    memcpy(buf + aLen, b.cstr(), bLen + 1);
    Data* data = getData();
    data->strByteLength = aLen + bLen;
    if (a.getData()->_strLength >= 0 && b.getData()->_strLength >= 0) {
        data->_strLength = a.length() + b.length();
    }
}

const String PGE::StringLiterals::operator""_PGE(const char* cstr, size_t size) {
    return String(cstr, size);
}

const String PGE::StringLiterals::operator""_PGE(const char8_t* cstr, size_t size) {
    return String((char*)cstr, size);
}

const String PGE::StringLiterals::operator""_PGE(const char16* wstr, size_t) {
    return String(wstr);
}


//
// Private constructors.
//

String::String(int size) {
    reallocate(size);
}

// Literal, size is WITHOUT terminating null byte!
String::String(const char* cstr, size_t size) {
    internalData.emplace<Literal>();
    std::get<Literal>(internalData).chs = (char*)cstr;
    getData()->strByteLength = (int)size;
}

// Byte substr.
String::String(const String& other, int from, int cnt) {
    reallocate(cnt);
    getData()->strByteLength = cnt;
    char* buf = cstrNoConst();
    memcpy(buf, other.cstr() + from, cnt);
    buf[cnt] = '\0';
}

void String::operator=(const String& other) {
    copy(*this, other);
}

void String::operator+=(const String& other) {
    int oldByteSize = byteLength();
    int newSize = oldByteSize + other.byteLength();
    reallocate(newSize, true);
    char* buf = cstrNoConst();
    memcpy(buf + oldByteSize, other.cstr(), other.byteLength() + 1);
    Data* data = getData();
    data->strByteLength = newSize;
    if (data->_strLength >= 0 && other.getData()->_strLength >= 0) {
        data->_strLength += other.length();
    } else {
        data->_strLength = -1;
    }
}

void String::operator+=(char16 ch) {
    int aLen = byteLength();
    reallocate(aLen + 4, true);
    Data* data = getData();
    char* buf = cstrNoConst();
    int actualSize = aLen + Unicode::wCharToUtf8(ch, buf + aLen);
    buf[actualSize] = '\0';
    data->strByteLength = actualSize;
    if (data->_strLength >= 0) {
        data->_strLength++;
    }
    data->_hashCode = 0; // TODO: Deal with partially evaluated hashcode.
}

const String PGE::operator+(const String& a, const String& b) {
    return String(a, b);
}

bool PGE::operator==(const String& a, const String& b) {
    return a.equals(b);
}

std::ostream& PGE::operator<<(std::ostream& os, const String& s) {
    return os.write(s.cstr(), s.byteLength());
}

// ASCII, fuck you.
std::istream& PGE::operator>>(std::istream& is, String& s) {
    // See MSVC xstring for reference.

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
    Data* data = getData();
    if (data->_hashCode == 0) {
        if (std::holds_alternative<Literal>(internalData)) {
            getOrAddLiteralData();
            if (data->_hashCode != 0) {
                return data->_hashCode;
            }
        }

        data->_hashCode = Hasher::getHash(std::span((byte*)cstr(), byteLength()));
    }
    return data->_hashCode;
}

const std::weak_ordering String::compare(const String& other) const {
    String::Iterator a = begin();
    String::Iterator b = other.begin();
    while (a != end() && b != other.end() && *a == *b) { a++; b++; }
    if (a == end()) {
        if (b == other.end()) {
            return std::weak_ordering::equivalent;
        } else {
            return std::weak_ordering::less;
        }
    } else if (b == other.end()) {
        return std::weak_ordering::greater;
    } else {
        return *a <=> *b;
    }
}

bool String::equals(const String& other) const {
    if (getChars() == other.getChars()) { return true; }
    if (byteLength() != other.byteLength()) { return false; }
    Data* data = getData(); Data* otherData = other.getData();
    if (data->_strLength >= 0 && otherData->_strLength >= 0 && length() != other.length()) { return false; }
    if (data->_hashCode != 0 && otherData->_hashCode != 0) { return getHashCode() == other.getHashCode(); }
    return memcmp(cstr(), other.cstr(), byteLength()) == 0;
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
    if (getChars() == other.getChars()) { return true; }
    if (getData()->_hashCode != 0 && other.getData()->_hashCode != 0 && getHashCode() == other.getHashCode()) { return true; }

    const char* buf[2] = { cstr(), other.cstr() };
    std::queue<char16> queue[2];

    // Feed first char.
    for (int i : Range(2)) {
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
        for (int i : Range(2)) {
            fold(buf[i], queue[i]);
        }
    }

    // If the strings are really equal, then both have the null char now.
    return *buf[0] == *buf[1];
}

bool String::isEmpty() const {
    return getChars()[0] == '\0';
}

void String::reallocate(int size, bool copyOldChs) {
    // Accounting for the terminating byte.
    size++;

    if (std::holds_alternative<Literal>(internalData)) {
        if (size <= SHORT_STR_CAPACITY) {
            Data* data = getData();
            int prevLen = data->strByteLength;
            char* chs = getChars();
            Unique& u = internalData.emplace<Unique>();
            if (copyOldChs) {
                memcpy(u.chs, chs, prevLen);
            }
            u.data = *data;
            return;
        }
    } else if (std::holds_alternative<Unique>(internalData)) {
        if (size <= SHORT_STR_CAPACITY) {
            return;
        }
    } else {
        std::shared_ptr<Shared>& s = std::get<std::shared_ptr<Shared>>(internalData);
        if (size <= s->cCapacity && s.use_count() == 1) {
            return;
        }
    }

    int targetCapacity = 1;
    while (targetCapacity < size) { targetCapacity <<= 1; }

    std::unique_ptr<char[]> newChs = std::make_unique<char[]>(targetCapacity);
    if (copyOldChs) {
        memcpy(newChs.get(), cstr(), getData()->strByteLength);
    }

    internalData = std::make_shared<Shared>();
    std::shared_ptr<Shared>& s = std::get<std::shared_ptr<Shared>>(internalData);
    s->chs = std::move(newChs);
    s->cCapacity = targetCapacity;
}

const char* String::cstr() const {
    return getChars();
}

const char8_t* String::c8str() const {
    return (const char8_t*)getChars();
}

char* String::cstrNoConst() {
    return getChars();
}

const std::vector<char16> String::wstr() const {
    std::vector<char16> chars;
    Data* data = getData();
    if (data->_strLength >= 0) {
        chars.reserve(data->_strLength);
    }
    // Convert all the codepoints to wchars.
    for (char16 ch : *this) {
        chars.emplace_back(ch);
    }
    chars.emplace_back(L'\0');
    return chars;
}

template <std::integral I>
static consteval byte maxIntegerDigits(byte base) {
    byte digits = 0;
    I max = std::numeric_limits<I>::max();
    while (max != 0) { digits++; max /= base; }
    if (std::numeric_limits<I>::is_signed) { digits++; }
    return digits;
}

template <std::integral I, byte BASE> requires ValidBaseForType<I, BASE>
const String String::fromInteger(I i, Casing casing) {
    constexpr byte digits = maxIntegerDigits<I>(BASE);
    String ret(digits);

    char* buf = ret.cstrNoConst();
    byte count = 0;
    if constexpr (std::numeric_limits<I>::is_signed) {
        if (i < 0) { buf[0] = '-'; buf++; count++; }
    }
    if (i == 0) { buf[0] = '0'; count = 1; }
    while (i != 0) {
        byte digit;
        if constexpr (std::numeric_limits<I>::is_signed) {
            digit = (byte)abs(i % BASE);
        } else {
            digit = i % BASE;
        }
        i /= BASE;
        if constexpr (BASE <= 10) {
            buf[count] = '0' + digit;
        } else {
            if (digit >= 10) {
                buf[count] = (casing == Casing::UPPER ? 'A' : 'a') + digit - 10;
            } else {
                buf[count] = '0' + digit;
            }
        }
        count++;
    }
    std::reverse(buf, buf + count);

    Data* retData = ret.getData();
    retData->strByteLength = count;
    retData->_strLength = retData->strByteLength;
    return ret;
}

static bool parseOptionalSign(String::Iterator& it, const String& str) {
    if (it != str.end()) {
        if (*it == L'-') {
            it++;
            return true;
        } else if (*it == L'+') {
            it++;
        }
    }
    return false;
}

template <char16 FIRST_CHAR, char16 LAST_CHAR, byte EXTRA = 0>
static bool charInRange(byte& digit, char16 ch) {
    if (ch < FIRST_CHAR || ch > LAST_CHAR) { return false; }
    digit = (byte)(ch - FIRST_CHAR + EXTRA);
    return true;
}

template <byte BASE>
static bool charToDigit(byte& digit, char16 ch) {
    if (charInRange<L'0', L'9'>(digit, ch)) { return true; }

    if constexpr (BASE > 10) {
        if (charInRange<L'A', L'A' + BASE - 11, 10>(digit, ch)) { return true; }
        if (charInRange<L'a', L'a' + BASE - 11, 10>(digit, ch)) { return true; }
    }

    return false;
}

template <std::integral I, byte BASE = 10>  requires ValidBaseForType<I, BASE>
static I toInteger(const String& str, bool& success) {
    success = false;
    I ret = 0;
    String::Iterator it = str.begin();
    bool neg = false;
    if constexpr (std::numeric_limits<I>::is_signed) {
        neg = parseOptionalSign(it, str);
    }
    for (; it != str.end(); it++) {
        char16 ch = *it;
        if (ret > std::numeric_limits<I>::max() / BASE) { return 0; }
        ret *= BASE;
        byte digit;
        if (!charToDigit<BASE>(digit, ch)) { return 0; }
        if (ret > std::numeric_limits<I>::max() - digit) { return 0; }
        if constexpr (std::numeric_limits<I>::is_signed) {
            if (neg && ret < std::numeric_limits<I>::min() + digit) { return 0; }
            ret += neg ? -digit : digit;
        } else {
            ret += digit;
        }
    }
    success = true;
    return ret;
}

#define PGE_STRING_TO_FROM_SIGNED_INTEGER(TYPE) \
template <> const TYPE String::to(bool& success) const { return toInteger<TYPE>(*this, success); } \
template <> const String String::from(const TYPE& t) { return fromInteger(t); }

#define PGE_STRING_TO_FROM_UNSIGNED_INTEGER(TYPE) \
PGE_STRING_TO_FROM_SIGNED_INTEGER(TYPE) \
template <> TYPE String::binToInt(bool& success) const { return toInteger<TYPE, 2>(*this, success); } \
template <> TYPE String::octToInt(bool& success) const { return toInteger<TYPE, 8>(*this, success); } \
template <> TYPE String::hexToInt(bool& success) const { return toInteger<TYPE, 16>(*this, success); } \
template <> const String String::binFromInt(TYPE t) { return fromInteger<TYPE, 2>(t); } \
template <> const String String::octFromInt(TYPE t) { return fromInteger<TYPE, 8>(t); } \
template <> const String String::hexFromInt(TYPE t, Casing casing) { return fromInteger<TYPE, 16>(t, casing); }

PGE_STRING_TO_FROM_SIGNED_INTEGER(short)
PGE_STRING_TO_FROM_SIGNED_INTEGER(int)
PGE_STRING_TO_FROM_SIGNED_INTEGER(long)
PGE_STRING_TO_FROM_SIGNED_INTEGER(long long)
PGE_STRING_TO_FROM_UNSIGNED_INTEGER(byte)
PGE_STRING_TO_FROM_UNSIGNED_INTEGER(unsigned short)
PGE_STRING_TO_FROM_UNSIGNED_INTEGER(unsigned int)
PGE_STRING_TO_FROM_UNSIGNED_INTEGER(unsigned long)
PGE_STRING_TO_FROM_UNSIGNED_INTEGER(unsigned long long)

template <std::floating_point F>
const String String::fromFloatingPoint(F f) {
    const char* format;
    if constexpr (std::is_same<F, long double>::value) {
        format = "%fL";
    } else {
        // floats are automatically promoted to double by printf
        format = "%f";
    }
    
    int size = snprintf(nullptr, 0, format, f);
    String ret(size + 1);
    snprintf(ret.cstrNoConst(), size + 1, format, f);

    Data* retData = ret.getData();
    retData->strByteLength = size;
    retData->_strLength = size;

    return ret;
}

static const String POSITIVE_INFINITY = "Inf";
static const String NEGATIVE_INFINITY = '-' + POSITIVE_INFINITY;
static const String POSITIVE_INFINITY_LONG = "Infinity";
static const String NEGATIVE_INFINITY_LONG = '-' + POSITIVE_INFINITY_LONG;

static const String NAN_STRING = "NaN";
static const String NAN_STRING_LOWER = NAN_STRING.toLower();

static constexpr char16 DECIMAL_SEPERATOR = L'.';

template <std::floating_point F>
static F toFloatingPoint(const String& str, bool& success) {
    success = true;

    // TODO: Move to end?
    if (str.equalsIgnoreCase(NAN_STRING_LOWER)) {
        return std::numeric_limits<F>::quiet_NaN();
    }

    if (str.equalsIgnoreCase(POSITIVE_INFINITY)
        || str.equalsIgnoreCase(POSITIVE_INFINITY_LONG)
        || str.equalsIgnoreCase(L"\u221E")) {
        return std::numeric_limits<F>::infinity();
    }

    if (str.equalsIgnoreCase(NEGATIVE_INFINITY)
        || str.equalsIgnoreCase(NEGATIVE_INFINITY_LONG)
        || str.equalsIgnoreCase(L"-\u221E")) {
        return -std::numeric_limits<F>::infinity();
    }

    success = false;
    F ret = 0;
    String::Iterator it = str.begin();
    bool neg = parseOptionalSign(it, str);

    for (; it != str.end(); it++) {
        char16 ch = *it;
        if (ch == DECIMAL_SEPERATOR) { break; }
        if (!Unicode::isDigit(ch)) { break; }
        ret *= 10;
        ret += (F)(ch - L'0');
    }

    if (it != str.end() && *it == DECIMAL_SEPERATOR) {
        it++;
        F multi = (F)1;
        for (; it != str.end(); it++) {
            multi /= 10;
            char16 ch = *it;
            if (!Unicode::isDigit(ch)) { break; }
            ret += (F)(ch - L'0') * multi;
        }
    }

    if (it != str.end() && (*it == L'e' || *it == L'E')) {
        it++;
        using Exponent = u32; // TODO: Type.
        Exponent exponent = 0;
        bool expNeg = parseOptionalSign(it, str);
        for (; it != str.end(); it++) {
            char16 ch = *it;
            if (!Unicode::isDigit(ch)) { return 0; }
            if (exponent > std::numeric_limits<Exponent>::max() / 10) { return 0; }
            exponent *= 10;
            Exponent digit = (Exponent)(ch - L'0');
            if (exponent > std::numeric_limits<Exponent>::max() - digit) { return 0; }
            exponent += digit;
        }
        if (expNeg) {
            ret = (F)(ret / pow(10, exponent));
        } else {
            ret = (F)(ret * pow(10, exponent));
        }
    }

    success = true;
    return neg ? -ret : ret;
}

#define PGE_STRING_TO_FLOAT(TYPE) \
template <> const TYPE String::to(bool& success) const { return toFloatingPoint<TYPE>(*this, success); } \
template <> const String String::from(const TYPE& t) { return fromFloatingPoint(t); }

PGE_STRING_TO_FLOAT(float)
PGE_STRING_TO_FLOAT(double)
PGE_STRING_TO_FLOAT(long double)

int String::length() const {
    Data* data = getData();
    if (data->_strLength < 0) {
        if (std::holds_alternative<Literal>(internalData)) {
            getOrAddLiteralData();
            if (data->_strLength >= 0) {
                return data->_strLength;
            }
        }

        const char* buf = cstr();
        data->_strLength = 0;
        for (int i = 0; buf[i] != '\0'; i += Unicode::measureCodepoint(buf[i])) {
            data->_strLength++;
        }
    }
    return data->_strLength;
}

int String::byteLength() const {
    Data* data = getData();
    asrt(data->strByteLength >= 0, "String byte length must always be valid");
    return data->strByteLength;
}

bool String::contains(const String& fnd) const {
    return findFirst(fnd) != begin();
}

const String::Iterator String::findFirst(const String& fnd, int from) const {
    return findFirst(fnd, begin() + from);
}

static const String EMPTY_FIND = "Find string can't be empty";

const String::Iterator String::findFirst(const String& fnd, const Iterator& from) const {
    if (fnd.isEmpty()) { return from; }
    for (String::Iterator it = from; it.getBytePosition() <= byteLength() - fnd.byteLength(); it++) {
        if (memcmp(fnd.cstr(), cstr() + it.getBytePosition(), fnd.byteLength()) == 0) {
            return it;
        }
    }
    return end();
}

const String::ReverseIterator String::findLast(const String& fnd, int fromEnd) const {
    return findLast(fnd, rbegin() + fromEnd);
}

const String::ReverseIterator String::findLast(const String& fnd, const ReverseIterator& from) const {
    asrt(!fnd.isEmpty(), EMPTY_FIND);
    String::ReverseIterator it = from;
    while (it.getBytePosition() > byteLength() - fnd.byteLength()) {
        it++;
    }
    for (; it != rend(); it++) {
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
    asrt(start.getBytePosition() <= to.getBytePosition(),
        "start iterator can't come after to iterator (start: " + from(start.getBytePosition())
        + "; to: " + from(to.getBytePosition()) + "; str: " + *this + ")");

    int newSize = to.getBytePosition() - start.getBytePosition();
    String retVal(newSize);
    Data* retData = retVal.getData();
    retData->strByteLength = newSize;
    // Due to not being friends with Iterators, we just bite the bullet here and hope for the best.
    retData->_strLength = to.getPosition() - start.getPosition();
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
    std::vector<int> foundPositions;
    for (String::Iterator it = findFirst(fnd); it != end(); it = findFirst(fnd, it + 1)) {
        foundPositions.emplace_back(it.getBytePosition());
    }
    if (fnd.isEmpty()) { foundPositions.emplace_back(byteLength()); }
    
    int newSize = byteLength() + (int)foundPositions.size() * (rplace.byteLength() - fnd.byteLength());
    String retVal(newSize);
    Data* retData = retVal.getData();
    retData->strByteLength = newSize;

    char* retBuf = retVal.cstrNoConst();
    int retPos = 0;
    int thisPos = 0;
    for (int pos : foundPositions) {
        int thisLen = pos - thisPos;
        memcpy(retBuf + retPos, cstr() + thisPos, thisLen);
        retPos += thisLen;
        memcpy(retBuf + retPos, rplace.cstr(), rplace.byteLength());
        retPos += rplace.byteLength();
        thisPos = pos + fnd.byteLength();
    }
    // Append the rest of the string, including terminating byte.
    memcpy(retBuf + retPos, cstr() + thisPos, byteLength() - thisPos + 1);
   
    Data* data = getData();

    // If the string that is being operated on already has had its length calculated, we assume it to be worth it to pre-calculate the new string's length.
    if (data->_strLength >= 0) {
        retData->_strLength = data->_strLength + (int)foundPositions.size() * (rplace.length() - fnd.length());
    }
    return retVal;
}

struct String::CharVisitor {
    char* operator()(const Unique& u) {
        return (char*)u.chs;
    }
    char* operator()(const std::shared_ptr<Shared>& ptr) {
        return ptr->chs.get();
    }
    char* operator()(const Literal& lit) {
        return lit.chs;
    }
};

char* String::getChars() const {
    return std::visit(CharVisitor(), internalData);
}

struct String::DataVisitor {
    Data* operator()(const Unique& u) {
        return (Data*)&u.data;
    }
    Data* operator()(const std::shared_ptr<Shared>& ptr) {
        return &ptr->data;
    }
    Data* operator()(const Literal& lit) {
        return std::holds_alternative<Data>(lit.data) ? (Data*)&std::get<Data>(lit.data) : std::get<Data*>(lit.data);
    }
};

String::Data* String::getData() const {
    return std::visit(DataVisitor(), internalData);
}

void String::getOrAddLiteralData() const {
    static std::unordered_map<const char*, String::Data> litData;
    static std::mutex litMut;

    char* chs = getChars();

    Data* data;
    const auto& it = litData.find(chs);
    if (it != litData.end()) {
        data = &it->second;
    } else {
        int litSize = getData()->strByteLength;
        { // This needs to be synced.
            std::lock_guard lock(litMut);
            data = &litData.emplace(
                chs,
                Data {
                    .strByteLength = litSize,
                }
            ).first->second;
        }
    }
    std::get<Literal>(internalData).data.emplace<Data*>(data);
}

// TODO: Funny special cases!
const String String::performCaseConversion(const std::function<void (String&, char16)>& func) const {
    String ret(byteLength());
    Data* retData = ret.getData();
    retData->strByteLength = 0;
    retData->_strLength = 0;
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
    if (isEmpty()) { return *this; }

    int newLen = getData()->_strLength;
    Iterator leading = begin();
    while (leading != end() && Unicode::isSpace(*leading)) {
        leading++; newLen--;
    }
    if (leading == end()) {
        return String();
    }

    ReverseIterator trailing = rbegin();
    while (Unicode::isSpace(*trailing)) { trailing++; newLen--; }

    String ret(*this, leading.getBytePosition(), trailing.getBytePosition() - leading.getBytePosition() + 1);
    ret.getData()->_strLength = newLen;
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
    Data* retData = ret.getData();
    retData->strByteLength = len;
    retData->_strLength = getData()->_strLength;
    return ret;
}

const String String::repeat(int count, const String& separator) const {
    asrt(count >= 0, "count must be non-negative");
    if (count == 0) { return String(); }
    int curLength = byteLength();
    int sepLength = separator.byteLength();
    int newLength = curLength * count + sepLength * (count - 1);
    String ret(newLength);
    char* buf = ret.cstrNoConst();
    buf[newLength] = '\0';
    for (int i : Range(count)) {
        if (i != 0) {
            memcpy(buf, separator.cstr(), sepLength);
            buf += sepLength;
        }
        memcpy(buf, cstr(), curLength);
        buf += curLength;
    }
    Data* retData = ret.getData();
    retData->strByteLength = newLength;
    if (getData()->_strLength >= 0) { retData->_strLength = length() * count + separator.length() * (count - 1); }
    return ret;
}

const std::vector<String> String::split(const String& needleStr, bool removeEmptyEntries) const {
    std::vector<String> split;
    const char* haystack = cstr();
    int codepoint;
    int needleLen = needleStr.byteLength();
    int cut = 0;
    for (int i = 0; i <= byteLength() - needleLen;) {
        codepoint = Unicode::measureCodepoint(haystack[i]);
        if (needleStr.isEmpty() || memcmp(haystack + i, needleStr.cstr(), needleLen) == 0) {
            int addSize = i - cut;
            if (addSize > 0) {
                split.emplace_back(String(*this, cut, addSize));
            } else if (!removeEmptyEntries) {
                split.emplace_back();
            }
            cut = i + needleLen;
            if (needleLen > codepoint) {
                i += needleLen;
                continue;
            }
        }
        i += codepoint;
    }
    // Add the rest of the string to the vector.
    int endAddSize = byteLength() - cut;
    if (!removeEmptyEntries || endAddSize > 0) {
        split.emplace_back(String(*this, cut, endAddSize));
    }
    return split;
}

const String String::regexMatch(const String& pattern) const {
    // TODO: C++23 will add support for char8_t and/or deprecate std::regex
    // We will have to react accordingly. Perhaps implementing our own regex matcher.
    std::vector<char16> s = wstr();
    std::wcmatch m;
    std::regex_search(s.data(), m, std::wregex(pattern.wstr().data()));

    s[m.position() + m.length()] = '\0';
    String ret(s.data() + m.position());
    return ret;
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
            for (int j : Range(codepoint)) {
                retBuf[i + j] = buf[i + j];
            }
        }
    }
    retBuf[resultSize] = '\0';
    ret.strByteLength = resultSize;
    return ret;
}*/

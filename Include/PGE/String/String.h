#ifndef PGE_STRING_H_INCLUDED
#define PGE_STRING_H_INCLUDED

#include <unordered_map>
#include <functional>
#include <vector>
#include <string>
#include <regex>
#include <variant>

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/NSString.h>
#endif

#include <PGE/Types/Types.h>
#include <PGE/Types/Range.h>

namespace PGE {

// 10 digits + 26 characters = 36
// Only unsigned numbers can be represented in other bases.
template <typename T, byte BASE>
concept ValidBaseForType = BASE == 10 && std::integral<T> || std::unsigned_integral<T> && (BASE >= 2 && BASE < 36);

class String;

inline namespace StringLiterals {
    const String operator""_PGE(const char* cstr, size_t size);
    const String operator""_PGE(const char8_t* cstr, size_t size);
    const String operator""_PGE(const char16 * wstr, size_t size);
}

/// A UTF-8 character sequence guaranteed to be terminated by a null byte.
class String {
    private:
        class BasicIterator {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using difference_type = int;
                using value_type = char16;
                using pointer = value_type*;
                using reference = value_type&;

                BasicIterator() = default;

                int operator-(const BasicIterator& other) const;

                char16 operator*() const;

                bool operator==(const BasicIterator& other) const;

                int getBytePosition() const;
                int getPosition() const;

            protected:
                void increment();
                void decrement();

                const String* ref = nullptr;
                // Lazily evaluated, Unicode invalid character by default.
                mutable char16 _ch = L'\uFFFF';
                mutable int charIndex;
                int index;
        };

        template <bool REVERSE>
        class ActualIterator : public BasicIterator {
            friend ActualIterator<!REVERSE>;

            private:
                ActualIterator(const String& str, int byteIndex, int chIndex) {
                    ref = &str;
                    index = byteIndex;
                    charIndex = chIndex;
                }
                
                void validate();

            public:
                ActualIterator() = default;

                ActualIterator(const ActualIterator<!REVERSE>& reversed) {
                    ref = reversed.ref;
                    _ch = reversed._ch;
                    charIndex = reversed.charIndex;
                    index = reversed.index;
                    validate();
                }

                static const ActualIterator begin(const String& str);
                static const ActualIterator end(const String& str);

                // These need to return what they return to be recognized as iterators by the STL.
                ActualIterator& operator++();
                ActualIterator& operator--();
                ActualIterator operator++(int) { ActualIterator temp = *this; ++*this; return temp; }
                ActualIterator operator--(int) { ActualIterator temp = *this; --*this; return temp; }

                const ActualIterator operator+(int steps) const { ActualIterator ret(*this); ret += steps; return ret; }
                const ActualIterator operator-(int steps) const { ActualIterator ret(*this); ret -= steps; return ret; }
                void operator+=(int steps) {
                    if (steps < 0) { *this -= -steps; }
                    for (PGE_IT : Range(steps)) {
                        ++(*this);
                    }
                }
                void operator-=(int steps) {
                    if (steps < 0) { *this += -steps; }
                    for (PGE_IT : Range(steps)) {
                        --(*this);
                    }
                }
        };

    public:
        struct Key;
        struct RedundantKey;
        struct SafeKey;
        struct OrderedKey;

        using Iterator = ActualIterator<false>;
        using ReverseIterator = ActualIterator<true>;
        static_assert(std::bidirectional_iterator<Iterator>);
        static_assert(std::bidirectional_iterator<ReverseIterator>);

        const Iterator begin() const;
        const Iterator end() const;
        const ReverseIterator rbegin() const;
        const ReverseIterator rend() const;
        
        String();
        String(const String& other);

        template <size_t S>
        String(const char(&cstri)[S])
            : String(cstri, S - 1) { }

        template <typename T, typename = typename std::enable_if<
            std::conjunction<
                std::is_pointer<T>,
                std::disjunction<
                    std::is_same<char*, T>,
                    std::is_same<const char*, T>
                >
            >::value
        >::type>
        String(T cstri) {
            int len = (int)strlen(cstri);
            reallocate(len);
            data->strByteLength = len;
            memcpy(cstrNoConst(), cstri, len + 1);
        }

        String(const char8_t* cstr);
        String(const char16* wstr);

        String(const std::string& cppstr);
#if defined(__APPLE__) && defined(__OBJC__)
        String(const NSString* nsstr);
#endif
        String(char c);
        String(char8_t c);
        String(char16 w);

        String(const String& a, const String& b);

        friend const String StringLiterals::operator""_PGE(const char* cstr, size_t size);
        friend const String StringLiterals::operator""_PGE(const char8_t* cstr, size_t size);
        friend const String StringLiterals::operator""_PGE(const char16* wstr, size_t size);

        template <typename T>
        static const String from(const T& t);

        enum class Casing {
            UPPER,
            LOWER,
        };

        template <std::unsigned_integral I> static const String binFromInt(I i);
        template <std::unsigned_integral I> static const String octFromInt(I i);
        template <std::unsigned_integral I> static const String hexFromInt(I i, Casing casing = Casing::UPPER);

        void operator=(const String& other);
        void operator+=(const String& other);
        void operator+=(char16 ch);

        /// Gets the UTF-8 data the string is composed of, without transferring ownership.
        /// Guaranteed to have a null byte appended to the string's content.
        /// 
        /// O(1)
        const char* cstr() const;
        const char8_t* c8str() const;
        const std::vector<char16> wstr() const;

        template <typename T> const T to(bool& success) const;
        template <typename T>
        const T to() const {
            bool succ;
            T t = to<T>(succ);
            // TODO: C++20 Modules.
            //asrt(succ, "Failed to convert");
            return t;
        }

        template <std::unsigned_integral I> I binToInt(bool& success) const;
        template <std::unsigned_integral I>
        I binToInt() const {
            bool succ;
            I t = binToInt<I>(succ);
            //asrt(succ, "Failed to convert");
            return t;
        }

        template <std::unsigned_integral I> I octToInt(bool& success) const;
        template <std::unsigned_integral I>
        I octToInt() const {
            bool succ;
            I t = octToInt<I>(succ);
            //asrt(succ, "Failed to convert");
            return t;
        }

        template <std::unsigned_integral I> I hexToInt(bool& success) const;
        template <std::unsigned_integral I>
        I hexToInt() const {
            bool succ;
            I t = hexToInt<I>(succ);
            //asrt(succ, "Failed to convert");
            return t;
        }

        /// The amount of characters the string is composed of, excluding the terminating null byte.
        /// 
        /// First call on same data: O(n), successive calls: O(1)
        int length() const;
        /// The amount of bytes the string is composed of, excluding the terminating null byte.
        /// Not necessarily equal to #length() when dealing with non-ASCII characters.
        /// 
        /// O(1)
        int byteLength() const;

        bool contains(const String& fnd) const;

        const Iterator findFirst(const String& fnd, int from = 0) const;
        const Iterator findFirst(const String& fnd, const Iterator& from) const;
        const ReverseIterator findLast(const String& fnd, int fromEnd = 0) const;
        const ReverseIterator findLast(const String& fnd, const ReverseIterator& from) const;

        const String substr(int start) const;
        const String substr(int start, int cnt) const;
        const String substr(const Iterator& start) const;
        const String substr(const Iterator& start, const Iterator& to) const;
        const Iterator charAt(int pos) const;
        const String replace(const String& fnd, const String& rplace) const;
        const String toUpper() const;
        const String toLower() const;
        const String trim() const;
        const String reverse() const;
        const String repeat(int count, const String& separator = "") const;
        const std::vector<String> split(const String& needleStr, bool removeEmptyEntries) const;
        static const String join(const Enumerable<String> auto& vect, const String& separator) {
            if (std::ranges::empty(vect)) {
                return String();
            }

            auto it = std::ranges::begin(vect);

            String retVal = *it;
            it++;
            for (; it != std::ranges::end(vect); it++) {
                retVal += separator + *it;
            }

            return retVal;
        }

        const String regexMatch(const String& pattern) const;

        //String unHex() const;

        u64 getHashCode() const;

        const std::weak_ordering compare(const String& other) const;

        bool equals(const String& other) const;
        bool equalsIgnoreCase(const String& other) const;
        bool isEmpty() const;

    private:
        String(int size);
        String(const char* cstr, size_t size);
        String(const String& other, int from, int cnt);

        static void copy(String& dst, const String& src);

        static constexpr int SHORT_STR_CAPACITY = 16;

        struct Data {
            // Lazily evaluated.
            mutable u64 _hashCode = 0;
            mutable int _strLength = -1;

            int strByteLength = -1;
        };

        struct Shared {
            Data data;
            int cCapacity;
            std::unique_ptr<char[]> chs;
        };

        struct Unique {
            Data data;
            char chs[SHORT_STR_CAPACITY];
        };

        struct Literal {
            std::variant<Data, Data*> data;
        };

        // Default initialized with Unique.
        mutable std::variant<Unique, std::shared_ptr<Shared>, Literal> internalData;
        char* chs = std::get<Unique>(internalData).chs;
        mutable Data* data = &std::get<Unique>(internalData).data;

        void getOrAddLiteralData() const;

        const String performCaseConversion(const std::function<void(String&, char16)>& func) const;

        void wCharToUtf8Str(const char16* wbuffer);
        void reallocate(int size, bool copyOldChs = false);
        char* cstrNoConst();

        template <std::integral I, byte BASE = 10> requires ValidBaseForType<I, BASE>
        static const String fromInteger(I i, Casing casing = Casing::UPPER);
        template <std::floating_point F>
        static const String fromFloatingPoint(F f);
};
const String operator+(const String& a, const String& b);
bool operator==(const String& a, const String& b);
std::ostream& operator<<(std::ostream& os, const String& s);
std::istream& operator>>(std::istream& is, String& s);

static_assert(std::ranges::bidirectional_range<String>);
static_assert(std::ranges::common_range<String>);

}

#endif // PGE_STRING_H_INCLUDED

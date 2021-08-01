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

#include <PGE/ResourceManagement/NoHeap.h>
#include <PGE/Types/Types.h>

namespace PGE {

/// A UTF-8 character sequence guaranteed to be terminated by a null byte.
class String : private NoHeap {
    public:
        struct Key;
        struct RedundantKey;
        struct SafeKey;
        struct OrderedKey;

        class Iterator : NoHeap {
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = int;
            using value_type = char16;
            using pointer = value_type*;
            using reference = value_type&;

            public:
                static const Iterator begin(const String& str);
                static const Iterator end(const String& str);

                Iterator();

                Iterator& operator++();
                Iterator& operator--();
                const Iterator operator++(int);
                const Iterator operator--(int);

                const Iterator operator+(int steps) const;
                const Iterator operator-(int steps) const;
                Iterator& operator+=(int steps);
                Iterator& operator-=(int steps);

                int operator-(const Iterator& other) const;

                char16 operator*() const;

                bool operator>(const Iterator& other) const;
                bool operator<(const Iterator& other) const;
                bool operator>=(const Iterator& other) const;
                bool operator<=(const Iterator& other) const;
                bool operator==(const Iterator& other) const;
                bool operator!=(const Iterator& other) const;

                int getBytePosition() const;
                int getPosition() const;

            protected:
                Iterator(const String& str, int byteIndex, int chIndex);

                void increment();
                void decrement();

                const String* ref;
                // Lazily evaluated, Unicode invalid character by default.
                mutable char16 _ch = L'\uFFFF';
                mutable int charIndex;
                int index;
        };

        struct ReverseIterator : public Iterator {
            ReverseIterator(const Iterator& it) : Iterator(it) { }

            ReverseIterator& operator++();
            ReverseIterator& operator--();
            const ReverseIterator operator++(int);
            const ReverseIterator operator--(int);

            const ReverseIterator operator+(int steps) const { return Iterator::operator-(steps); }
            const ReverseIterator operator-(int steps) const { return Iterator::operator+(steps); }
            ReverseIterator& operator+=(int steps) { Iterator::operator-=(steps); return *this; }
            ReverseIterator& operator-=(int steps) { Iterator::operator+=(steps); return *this; }

            bool operator>(const Iterator& other) const { return Iterator::operator<(other); }
            bool operator<(const Iterator& other) const { return Iterator::operator>(other); }
            bool operator>=(const Iterator& other) const { return Iterator::operator<=(other); }
            bool operator<=(const Iterator& other) const { return Iterator::operator>=(other); }

            static const ReverseIterator rbegin(const String& str);
            static const ReverseIterator rend(const String& str);

            private:
                using Iterator::Iterator;
                using Iterator::begin;
                using Iterator::end;
        };

        const Iterator begin() const;
        const Iterator end() const;
        const ReverseIterator rbegin() const;
        const ReverseIterator rend() const;
        
        String();
        String(const String& other);

        template <size_t S>
        String(const char(&cstri)[S])
            : internalData(std::monostate()), chs((char*)cstri), data(nullptr) {
            initLiteral((int)S);
        }

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

        String(const std::string& cppstr);
        String(const char16* wstr);
#if defined(__APPLE__) && defined(__OBJC__)
        String(const NSString* nsstr);
#endif
        String(char c);
        String(char16 w);

        template <typename T>
        static const String format(T t, const String& format);
        static const String fromInt(int i);
        static const String fromFloat(float f);

        String& operator=(const String& other);
        String& operator+=(const String& other);
        String& operator+=(char16 ch);

        // TODO: Remove (juan hates his friends).
        friend const String operator+(const String& a, const String& b);
        friend const String operator+(const char* a, const String& b);
        friend const String operator+(const String& a, const char* b);
        friend const String operator+(const String& a, char16 b);
        friend const String operator+(char16 a, const String& b);

        /// Gets the UTF-8 data the string is composd of, without transferring ownership.
        /// Guaranteed to have a null byte appended to the string's content.
        /// 
        /// O(1)
        const char* cstr() const;
        std::vector<char16> wstr() const;
        int toInt(bool& success) const;
        float toFloat(bool& success) const;
        int toInt() const;
        float toFloat() const;

        /// The amount of characters the string is composed of, excluding the terminating null byte.
        /// 
        /// First call on same data: O(n), successive calls: O(1)
        int length() const;
        /// The amount of bytes the string is composed of, excluding the terminating null byte.
        /// Not necessarily equal to #length() when dealing with non-ASCII characters.
        /// 
        /// O(1)
        int byteLength() const;

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
        const String multiply(int count, const String& separator = "") const;
        std::vector<String> split(const String& needleStr, bool removeEmptyEntries) const;
        static const String join(const std::vector<String>& vect, const String& separator);

        const std::cmatch regexMatch(const std::regex& pattern) const;

        //String unHex() const;

        u64 getHashCode() const;

        bool equals(const String& other) const;
        bool equalsIgnoreCase(const String& other) const;
        bool isEmpty() const;

    private:
        static constexpr u64 FNV_SEED = 0xcbf29ce484222325;

        String(int size);
        String(const String& other, int from, int cnt);

        static void copy(String& dst, const String& src);

        static constexpr int SHORT_STR_CAPACITY = 16;

        struct Data {
            int strByteLength = -1;

            int cCapacity = SHORT_STR_CAPACITY;

            // Lazily evaluated.
            mutable u64 _hashCode;
            mutable int _strLength = -1;
            mutable bool _hashCodeEvaluted = false;
        };

        struct Shared {
            Data data;
            std::unique_ptr<char[]> chs;
        };

        struct Unique {
            Data data;
            char chs[SHORT_STR_CAPACITY];
        };

        // Default initialized with Unique.
        std::variant<Unique, std::shared_ptr<Shared>, std::monostate> internalData;
        // TODO: Investigate whether calculating these instead is worth it.
        char* chs = std::get<Unique>(internalData).chs;
        Data* data = &std::get<Unique>(internalData).data;

        void initLiteral(int litSize);

        const String performCaseConversion(const std::function<void(String&, char16)>& func) const;

        void wCharToUtf8Str(const char16* wbuffer);
        void reallocate(int size, bool copyOldChs = false);
        char* cstrNoConst();
};
bool operator==(const String& a, const String& b);
bool operator!=(const String& a, const String& b);
std::ostream& operator<<(std::ostream& os, const String& s);
std::istream& operator>>(std::istream& is, String& s);

}

#endif // PGE_STRING_H_INCLUDED

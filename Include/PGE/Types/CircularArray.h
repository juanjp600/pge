#ifndef PGE_CIRCULARARRAY_H_INCLUDED
#define PGE_CIRCULARARRAY_H_INCLUDED

#include <ranges>

#include <PGE/Types/Range.h>
#include <PGE/Exception/Exception.h>

namespace PGE {

template <typename T, typename Allocator = std::allocator<T>>
class CircularArray {
    private:
        static constexpr int GROWTH_FACTOR = 4;

        Allocator alloc;
        T* elements = nullptr;
        size_t capacity = 0;
        size_t beginIndex = 0;
        size_t endIndex = 0;

        constexpr void deleteElems() {
            if constexpr (std::is_trivially_destructible<T>::value) {
                return;
            }
            
            for (size_t i : Range(beginIndex, endIndex)) {
                elements[i % capacity].~T();
            }
        }

        constexpr void copyContents(T* loc) const {
            if (endIndex > capacity) {
                copy(loc, beginIndex, capacity);
                copy(loc + capacity - beginIndex, 0, endIndex - capacity);
            } else {
                copy(loc, beginIndex, endIndex);
            }
        }

        constexpr void copy(T* dest, size_t from, size_t to) const {
            if constexpr (std::is_trivially_copyable<T>::value) {
                memcpy(dest, elements + from, (to - from) * sizeof(T));
            } else {
                std::move(elements + from, elements + to, dest);
            }
        }
        
        constexpr void reallocate(size_t cap) {
            T* newT = alloc.allocate(cap);
            copyContents(newT);
            alloc.deallocate(elements, capacity);
            elements = newT;
            endIndex = endIndex - beginIndex;
            beginIndex = 0;
            capacity = cap;
        }

        constexpr void assertNotEmpty(std::source_location loc = std::source_location::current()) const {
            asrt(size() != 0, "circular array was empty", loc);
        }

        constexpr T& frontInternal() const {
            assertNotEmpty();
            return elements[beginIndex];
        }

        constexpr T& backInternal() const {
            assertNotEmpty();
            return elements[(endIndex - 1) % capacity];
        }
        
        constexpr T& indexInternal(size_t index) const {
            asrt(index < size(), "index must be less than size");
            return elements[(beginIndex + index) % capacity];
        }

        template <bool CONST>
        class BasicIterator {
            private:
                using CArray = std::conditional<CONST, const CircularArray, CircularArray>::type;
                CArray* carr = nullptr;
                size_t pos;

                constexpr void assertPosValid(size_t newPos, const std::source_location& = std::source_location::current()) const {
                    asrt(newPos >= carr->beginIndex && newPos <= carr->endIndex, "invalid index");
                }

            public:
                using iterator_category = std::random_access_iterator_tag;
                using difference_type = i64;
                using value_type = T;
                using pointer = value_type*;
                using reference = value_type&;

                constexpr static const BasicIterator begin(CArray& carr) {
                    BasicIterator it;
                    it.carr = &carr;
                    it.pos = carr.beginIndex;
                    return it;
                }

                constexpr static const BasicIterator end(CArray& carr) {
                    BasicIterator it;
                    it.carr = &carr;
                    it.pos = carr.endIndex;
                    return it;
                }

                constexpr std::conditional<CONST, const T&, T&>::type operator*() const {
                    asrt(pos < carr->endIndex, "tried dereferencing end iterator");
                    return carr->elements[pos % carr->capacity];
                }

                constexpr BasicIterator& operator++() {
                    asrt(pos < carr->endIndex, "Tried incrementing end iterator");
                    pos++;
                    return *this;
                }

                constexpr BasicIterator& operator--() {
                    asrt(pos > carr->beginIndex, "Tried decrementing begin iterator");
                    pos--;
                    return *this;
                }

                constexpr BasicIterator operator++(int) { BasicIterator it = *this; ++*this; return it; }
                constexpr BasicIterator operator--(int) { BasicIterator it = *this; --*this; return it; }

                constexpr BasicIterator operator+(i64 off) const {
                    BasicIterator ret(*this);
                    ret += off;
                    return ret;
                }

                constexpr BasicIterator operator-(i64 off) const {
                    BasicIterator ret(*this);
                    ret -= off;
                    return ret;
                }

                constexpr friend BasicIterator operator+(i64 off, const BasicIterator& it) {
                    return it + off;
                }

                constexpr friend BasicIterator operator-(i64 off, const BasicIterator& it) {
                    return it - off;
                }

                constexpr BasicIterator& operator+=(i64 off) {
                    assertPosValid(pos + off);
                    pos += off;
                    return *this;
                }

                constexpr BasicIterator& operator-=(i64 off) {
                    assertPosValid(pos - off);
                    pos -= off;
                    return *this;
                }

                constexpr std::conditional<CONST, const T&, T&>::type operator[](i64 off) const {
                    return *(*this + off);
                }

                constexpr i64 operator-(const BasicIterator& other) const {
                    asrt(carr == other.carr, "Tried calculating iterator difference for different carrays");
                    return pos - other.pos;
                }

                constexpr size_t getPosition() const {
                    return pos - carr->beginIndex;
                }

                constexpr std::partial_ordering operator<=>(const BasicIterator& other) const {
                    if (carr != other.carr) { return std::partial_ordering::unordered; }
                    return pos <=> other.pos;
                }

                constexpr bool operator==(const BasicIterator& other) const {
                    return carr == other.carr && pos == other.pos;
                }
        };

    public:
        using Iterator = BasicIterator<false>;
        using ConstIterator = BasicIterator<true>;

        constexpr CircularArray() = default;

        constexpr CircularArray(const Allocator& alloc) : alloc(alloc) { }

        constexpr CircularArray(const CircularArray& other, const Allocator& alloc = Allocator())
            : alloc(alloc) {
            *this = other;
        }

        constexpr CircularArray(CircularArray&& other) noexcept {
            *this = std::move(other);
        }

        constexpr CircularArray(CircularArray&& other, const Allocator& alloc) noexcept
            : alloc(alloc) {
            *this = std::move(other);
        }

        constexpr CircularArray(size_t count, const T& value = T(), const Allocator& alloc = Allocator())
            : alloc(alloc) {
            reserve(count);
            for (size_t i : Range(count)) {
                new (elements + i) T(value);
            }
            endIndex = count;
        }

        constexpr CircularArray(const Enumerable<T> auto& ts, const Allocator& alloc = Allocator())
            : alloc(alloc) {
            *this = ts;
        }

        constexpr CircularArray(const std::initializer_list<T>& ts, const Allocator& alloc = Allocator())
            : alloc(alloc) {
            *this = ts;
        }

        constexpr ~CircularArray() {
            deleteElems();
            alloc.deallocate(elements, capacity);
        }

        constexpr void operator=(const CircularArray& other) {
            clear();
            reserve(other.size());
            other.copyContents(elements);
            endIndex = other.size();
        }

        constexpr void operator=(CircularArray&& other) noexcept {
            std::swap(alloc, other.alloc);
            std::swap(elements, other.elements);
            std::swap(capacity, other.capacity);
            std::swap(beginIndex, other.beginIndex);
            std::swap(endIndex, other.endIndex);
        }

        constexpr void operator=(const Enumerable<T> auto& ts) {
            clear();
            if constexpr (std::ranges::sized_range<decltype(ts)>) {
                size_t size = std::ranges::size(ts);
                reserve(size);
                if constexpr (std::is_trivially_copyable<T>::value && std::ranges::contiguous_range<decltype(ts)>) {
                    memcpy(elements, std::ranges::data(ts), size * sizeof(T));
                } else {
                    for (size_t i = 0; const T& t : ts) {
                        new (elements + i) T(t);
                        i++;
                    }
                }
                endIndex = size;
            } else {
                for (const T& t : ts) {
                    pushBack(t);
                }
            }
        }

        constexpr void operator=(const std::initializer_list<T>& ts) {
            this->operator=<std::initializer_list<T>>(ts);
        }

        constexpr auto operator<=>(const Enumerable<T> auto& ts) const {
            using Ordering = decltype(ts <=> ts);

            size_t i = beginIndex;
            for (const T& t : ts) {
                if (i >= endIndex) { return Ordering::less; }

                if ((elements[i % capacity] <=> t) != Ordering::equivalent) {
                    return elements[i % capacity] <=> t;
                }
                i++;
            }

            if (i == endIndex) {
                return Ordering::equivalent;
            } else {
                return Ordering::greater;
            }
        }

        constexpr bool operator==(const Enumerable<T> auto& ts) {
            return (*this <=> ts) == 0;
        }

        constexpr bool operator==(const std::initializer_list<T>& ts) {
            return (*this <=> ts) == 0;
        }

        constexpr Iterator begin() { return Iterator::begin(*this); }
        constexpr Iterator end() { return Iterator::end(*this); }
        constexpr ConstIterator begin() const { return cbegin(); }
        constexpr ConstIterator end() const { return cend(); }
        constexpr ConstIterator cbegin() const { return ConstIterator::begin(*this); }
        constexpr ConstIterator cend() const { return ConstIterator::end(*this); }

        using ReverseIterator = std::reverse_iterator<Iterator>;
        constexpr const ReverseIterator rbegin() { return ReverseIterator(Iterator::end(*this)); }
        constexpr const ReverseIterator rend() { return ReverseIterator(Iterator::begin(*this)); }

        using ReverseConstIterator = std::reverse_iterator<ConstIterator>;
        constexpr const ReverseConstIterator rbegin() const { return crbegin(); }
        constexpr const ReverseConstIterator rend() const { return crend(); }
        constexpr const ReverseConstIterator crbegin() const { return ReverseConstIterator(ConstIterator::end(*this)); }
        constexpr const ReverseConstIterator crend() const { return ReverseConstIterator(ConstIterator::begin(*this)); }

        template <typename... Args>
        constexpr T& emplaceFront(Args&&... args) {
            reserve(size() + 1);
            beginIndex = (beginIndex - 1 + capacity) % capacity;
            if (beginIndex > endIndex) { endIndex += capacity; }
            T& newT = *new (elements + beginIndex) T(std::forward<Args>(args)...);
            return newT;
        }

        template <typename... Args>
        constexpr T& emplaceBack(Args&&... args) {
            reserve(size() + 1);
            T& newT = *new (elements + (endIndex % capacity)) T(std::forward<Args>(args)...);
            endIndex++;
            return newT;
        }

        constexpr void pushFront(const T& t) {
            emplaceFront(t);
        }

        constexpr void pushBack(const T& t) {
            emplaceBack(t);
        }

        constexpr void pushFront(T&& t) {
            emplaceFront(std::move(t));
        }

        constexpr void pushBack(T&& t) {
            emplaceBack(std::move(t));
        }

        constexpr void popFront() {
            assertNotEmpty();
            elements[beginIndex].~T();
            beginIndex++;
            if (beginIndex >= capacity) {
                beginIndex %= capacity;
                endIndex %= capacity;
            }
        }

        constexpr void popBack() {
            assertNotEmpty();
            endIndex--;
            elements[endIndex % capacity].~T();
        }

        constexpr T& front() { return frontInternal(); }
        constexpr const T& front() const { return frontInternal(); }

        constexpr T& back() { return backInternal(); }
        constexpr const T& back() const { return backInternal(); }

        constexpr bool empty() const {
            return beginIndex == endIndex;
        }

        constexpr size_t size() const {
            return endIndex - beginIndex;
        }

        constexpr T& operator[](size_t index) { return indexInternal(index); }
        constexpr const T& operator[](size_t index) const { return indexInternal(index); }

        constexpr void reserve(size_t cap) {
            if (capacity >= cap) { return; }
            size_t newCap = capacity == 0 ? GROWTH_FACTOR : capacity;
            while (newCap < cap) { newCap *= GROWTH_FACTOR; }
            reallocate(newCap);
        }

        constexpr void clear() {
            deleteElems();
            beginIndex = 0;
            endIndex = 0;
        }
};

}

#endif // PGE_CIRCULARARRAY_H_INCLUDED

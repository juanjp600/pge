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
        size_t _size = 0;
        size_t beginIndex = 0;
        size_t endIndex = 0;

        constexpr void deleteElems() {
            if constexpr (std::is_trivially_destructible<T>::value) {
                return;
            }

            if (endIndex > beginIndex) {
                for (size_t i : Range(beginIndex, endIndex)) {
                    elements[i].~T();
                }
            } else if (beginIndex < endIndex) {
                for (size_t i : Range(beginIndex, capacity)) {
                    elements[i].~T();
                }
                for (size_t i : Range(endIndex)) {
                    elements[i].~T();
                }
            }
        }

        constexpr void copyContents(T* loc) const {
            if (endIndex > beginIndex) {
                copy(loc, beginIndex, endIndex);
            } else {
                copy(loc, beginIndex, capacity);
                copy(loc + capacity - beginIndex, 0, endIndex);
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
            beginIndex = 0;
            endIndex = _size;
            capacity = cap;
        }

        constexpr void assertNotEmpty(std::source_location loc = std::source_location::current()) const {
            asrt(_size != 0, "circular array was empty", loc);
        }

    public:
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
            _size = count;
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
            reserve(other._size);
            other.copyContents(elements);
            _size = other._size;
            endIndex = other._size;
        }

        constexpr void operator=(CircularArray&& other) noexcept {
            std::swap(alloc, other.alloc);
            std::swap(elements, other.elements);
            std::swap(capacity, other.capacity);
            std::swap(_size, other._size);
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
                _size = size;
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

        constexpr auto operator<=>(const Enumerable<T> auto& ts) {
            using Ordering = decltype(ts <=> ts);

            size_t i = 0;
            for (const T& t : ts) {
                if (i >= _size) { return Ordering::less; }

                if (((*this)[i] <=> t) != Ordering::equivalent) {
                    return (*this)[i] <=> t;
                }
                i++;
            }

            if (i == _size) {
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

        template <typename... Args>
        constexpr T& emplaceFront(Args&&... args) {
            reserve(capacity + 1);
            beginIndex = (beginIndex - 1 + capacity) % capacity;
            T& newT = *new (elements + beginIndex) T(std::forward<Args>(args)...);
            _size++;
            return newT;
        }

        template <typename... Args>
        constexpr T& emplaceBack(Args&&... args) {
            reserve(capacity + 1);
            T& newT = *new (elements + endIndex) T(std::forward<Args>(args)...);
            endIndex = (endIndex + 1) % capacity;
            _size++;
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
            beginIndex = (beginIndex + 1) % capacity;
            _size--;
        }

        constexpr void popBack() {
            assertNotEmpty();
            endIndex = (endIndex - 1 + capacity) % capacity;
            elements[endIndex].~T();
            _size--;
        }

        constexpr T& front() const {
            assertNotEmpty();
            return elements[beginIndex];
        }

        constexpr T& back() const {
            assertNotEmpty();
            return elements[(endIndex - 1 + capacity) % capacity];
        }

        constexpr bool empty() const {
            return _size == 0;
        }

        constexpr size_t size() const {
            return _size;
        }

        constexpr T& operator[](size_t index) const {
            asrt(index < _size, "index must be less than size");
            return elements[(beginIndex + index) % capacity];
        }

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
            _size = 0;
        }
};

}

#endif // PGE_CIRCULARARRAY_H_INCLUDED

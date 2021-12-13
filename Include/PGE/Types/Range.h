#ifndef PGE_RANGE_H_INCLUDED
#define PGE_RANGE_H_INCLUDED

#include <iterator>

#include <PGE/Types/Concepts.h>

#define PGE_INTERNAL_CONCAT(a, b) PGE_INTERNAL_CONCAT_INNER(a, b)
#define PGE_INTERNAL_CONCAT_INNER(a, b) a ## b

#ifdef __COUNTER__
#define PGE_IT [[maybe_unused]] const auto& PGE_INTERNAL_CONCAT(PGE_INTERNAL_CONCAT(_, __COUNTER__), _PGE_INTERNAL_ITERATOR_)
#else
#define PGE_IT [[maybe_unused]] const auto& PGE_INTERNAL_CONCAT(PGE_INTERNAL_CONCAT(_, __LINE__), _PGE_INTERNAL_ITERATOR_)
#endif

namespace PGE {

template <Arithmetic SIZE = int>
class Range {
	public:
		class Iterator {
			public:
				using difference_type = SIZE;
				using value_type = SIZE;
				using pointer = SIZE*;
				using reference = SIZE&;
				using iterator_category = std::random_access_iterator_tag;

				static constexpr Iterator begin(const Range& range) { return { range.start, range.step }; }
				static constexpr Iterator end(const Range& range) { return { range.stop, range.step }; }

				constexpr operator SIZE&() { return position; }

				constexpr const Iterator& operator*() const { return *this; }
				constexpr Iterator& operator*() { return *this; }
				constexpr SIZE operator[](SIZE offset) const { return position + step * offset; }
				constexpr SIZE operator-(const Iterator& other) const { return position - other.position; }

				constexpr void operator++() { position += step; }
				constexpr void operator--() { position -= step; }
				constexpr void operator++(int) { position += step; }
				constexpr void operator--(int) { position -= step; }
				constexpr void operator+=(SIZE steps) { position += step * steps; }
				constexpr void operator-=(SIZE steps) { position -= step * steps; }

				constexpr const Iterator operator+(SIZE steps) const { return Iterator(position + step * steps, step); }
				constexpr const Iterator operator-(SIZE steps) const { return Iterator(position - step * steps, step); }

				constexpr const std::weak_ordering operator<=>(const Iterator& other) const { return position <=> other.position; }

			private:
				constexpr Iterator(SIZE position, SIZE step) : position(position), step(step) { }

				SIZE position;
				SIZE step;
		};

		constexpr Range() = default;
		constexpr Range(SIZE end) : stop(end) { }
		constexpr Range(SIZE begin, SIZE end, SIZE step = 1) : start(begin), stop(end), step(step) { }

		constexpr Iterator begin() { return Iterator::begin(*this); }
		constexpr Iterator end() { return Iterator::end(*this); }

	private:	
		SIZE start = 0;
		SIZE stop = 0;
		SIZE step = 1;
};

}

#endif // PGE_RANGE_H_INCLUDED

#ifndef PGE_RANGE_H_INCLUDED
#define PGE_RANGE_H_INCLUDED

namespace PGE {

template <typename SIZE = int>
class Range {
	public:
		class RangeIterator {
			public:
				using difference_type = SIZE;
				using value_type = SIZE;
				using pointer = SIZE*;
				using reference = SIZE&;
				using iterator_category = std::random_access_iterator_tag;

				static constexpr RangeIterator begin(const Range& range) { return { range.start, range.step }; }
				static constexpr RangeIterator end(const Range& range) { return { range.stop, range.step }; }

				constexpr SIZE operator*() const { return position; }
				constexpr SIZE operator[](SIZE offset) const { return position + step * offset; }
				constexpr SIZE operator-(const RangeIterator& other) const { return position - other.position; }

				constexpr void operator++() { position += step; }
				constexpr void operator--() { position -= step; }
				constexpr void operator++(int) { position += step; }
				constexpr void operator--(int) { position -= step; }
				constexpr void operator+=(SIZE steps) { position += step * steps; }
				constexpr void operator-=(SIZE steps) { position -= step * steps; }

				constexpr const RangeIterator operator+(SIZE steps) const { return RangeIterator(position + step * steps, step); }
				constexpr const RangeIterator operator-(SIZE steps) const { return RangeIterator(position - step * steps, step); }

				// TODO: C++20 <=>
				constexpr bool operator<(const RangeIterator& other) const { return position < other.position; }
				constexpr bool operator>(const RangeIterator& other) const { return position > other.position; }
				constexpr bool operator<=(const RangeIterator& other) const { return position <= other.position; }
				constexpr bool operator>=(const RangeIterator& other) const { return position >= other.position; }
				constexpr bool operator==(const RangeIterator& other) const { return position == other.position; }
				constexpr bool operator!=(const RangeIterator& other) const { return position != other.position; }

			private:
				constexpr RangeIterator(SIZE position, SIZE step) : position(position), step(step) { }

				SIZE position;
				SIZE step;
		};

		constexpr Range() = default;
		constexpr Range(SIZE end) : stop(end) { }
		constexpr Range(SIZE begin, SIZE end, SIZE step = 1) : start(begin), stop(end), step(step) { }

		constexpr RangeIterator begin() { return RangeIterator::begin(*this); }
		constexpr RangeIterator end() { return RangeIterator::end(*this); }

	private:	
		SIZE start = 0;
		SIZE stop = 0;
		SIZE step = 1;
};

}

#endif // PGE_RANGE_H_INCLUDED

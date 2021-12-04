#ifndef PGE_HASHER_H_INCLUDED
#define PGE_HASHER_H_INCLUDED

#include <span>

namespace PGE {

// FNV-1a
// Public domain
class Hasher {
	public:
		constexpr void feed(std::integral auto value) {
			hash ^= value;
			hash *= 0x00000100000001b3u;
		}

		constexpr u64 getHash() const {
			return hash;
		}

		template <std::integral T>
		static constexpr u64 getHash(const std::span<T>& data) {
			Hasher hasher;
			for (T t : data) {
				hasher.feed(t);
			}
			return hasher.getHash();
		}

	private:
		u64 hash = 0xcbf29ce484222325;
};

}

#endif // PGE_HASHER_H_INCLUDED

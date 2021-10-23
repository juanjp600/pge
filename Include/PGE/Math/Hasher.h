#ifndef PGE_HASHER_H_INCLUDED
#define PGE_HASHER_H_INCLUDED

namespace PGE {

// FNV-1a
// Public domain
class Hasher {
	public:
		template <typename T>
		constexpr void feed(T value) {
			hash ^= value;
			hash *= 0x00000100000001b3u;
		}

		constexpr u64 getHash() const {
			return hash;
		}

		template <typename T>
		static constexpr u64 getHash(const T* data, size_t size) {
			Hasher hasher;
			for (size_t i = 0; i < size; i++) {
				hasher.feed(data[i]);
			}
			return hasher.getHash();
		}

	private:
		u64 hash = 0xcbf29ce484222325;
};

}

#endif // PGE_HASHER_H_INCLUDED

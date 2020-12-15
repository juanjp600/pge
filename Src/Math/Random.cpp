#include <Math/Random.h>
#include <time.h>

Random::Random() {
	stateCounter = 0;
	increment = 1;
}

Random::Random(uint64_t seed) {
	// Custom seeding, no idea if it makes much sense.
	stateCounter = seed;
	increment = seed;
}

const uint64_t max32Val = 0x100000000;

uint32_t Random::getRandom() {
	uint64_t x, y, z;
	y = x = stateCounter * 0xcf76a3d654978c2d;
	z = y + 0xcf76a3d654978c2d;
	
	x = x * x + y;
	x = (x >> 32) | (x << 32);
	
	x = x * x + z;
	x = (x >> 32) | (x << 32);
	
	x = x * x + y;
	x = (x >> 32) | (x << 32);

	stateCounter += increment;
	
	return (x * x + z) >> 32;
}

Random2::Random2() {
	state[0] = 1;
	state[1] = 0xFECFEC;
	state[2] = 666;
	state[3] = 0xAAAAAAAAA;
}

Random2::Random2(uint64_t seed[]) {
	for (int i = 0; i < 4; i++) {
		state[i] = seed[i];
	}
}

static inline uint32_t rotl(const uint32_t x, int k) {
	return (x << k) | (x >> (32 - k));
}

uint32_t Random2::getRandom() {
	uint32_t result = rotl(state[1] * 5, 7) * 9;

	uint32_t t = state[1] << 9;

	state[2] ^= state[0];
	state[3] ^= state[1];
	state[1] ^= state[2];
	state[0] ^= state[3];

	state[2] ^= t;

	state[3] = rotl(state[3], 11);

	return result;
}

// equivalent to 2^64 calls to next();
// 2^64 non-overlapping subsequences
void Random2::jump() {
	static const uint32_t JUMP[] = { 0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b };

	uint32_t s0 = 0;
	uint32_t s1 = 0;
	uint32_t s2 = 0;
	uint32_t s3 = 0;
	for (int i = 0; i < sizeof JUMP / sizeof * JUMP; i++)
		for (int b = 0; b < 32; b++) {
			if (JUMP[i] & UINT32_C(1) << b) {
				s0 ^= state[0];
				s1 ^= state[1];
				s2 ^= state[2];
				s3 ^= state[3];
			}
			getRandom();
		}

	state[0] = s0;
	state[1] = s1;
	state[2] = s2;
	state[3] = s3;
}

Random3::Random3() {
	state = 0;
	weyl = 0;
	seed = 0xb5ad4eceda1ce2a9;
}

Random3::Random3(uint64_t seed) {
	state = 0;
	weyl = 0;
	if (seed % 2 == 0) {
		seed++;
	}
	this->seed = seed;
}

uint32_t Random3::getRandom() {
	state *= state;
	state += (weyl += seed);
	return state = (state >> 32) | (state << 32);
}

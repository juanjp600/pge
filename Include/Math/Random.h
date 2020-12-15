#ifndef PGE_RANDOM_H_INCLUDED
#define PGE_RANDOM_H_INCLUDED

#include <cstdint>

// Squares CBRNG
// https://arxiv.org/abs/2004.06278v3
// GNU General Public License
class Random {
	public:
		Random();
		Random(uint64_t seed);

		uint32_t getRandom();

	private:
		uint64_t stateCounter;
		uint16_t increment;

};

// xoshiro128**
// http://prng.di.unimi.it/
// Public Domain
class Random2 {
	public:
		Random2();
		Random2(uint64_t seed[]);

		uint32_t getRandom();

	private:
		uint64_t state[4];

		void jump();
};

// Middle Square Weyl Sequence PRNG
// https://en.wikipedia.org/wiki/Middle-square_method#Middle_Square_Weyl_Sequence_PRNG
// no idea
class Random3 {
	public:
		Random3();
		Random3(uint64_t seed);

		uint32_t getRandom();

	private:
		uint64_t state;
		uint64_t weyl;
		uint64_t seed;
};

#endif // PGE_RANDOM_H_INCLUDED

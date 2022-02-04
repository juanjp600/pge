#include "Util.h"

#include <PGE/Math/Math.h>

using namespace PGE;

template <std::floating_point F, F value, F(*func)(F)>
void testFunc() {
	constexpr F c = func(value);
	CHECK(doctest::Approx(c) == func(value));
}

template <std::floating_point F, F value>
void testAllFuncs() {
	testFunc<F, value, Math::sin>();
	testFunc<F, value, Math::cos>();
	if constexpr (value > 0) {
		testFunc<F, value, Math::sqrt>();
	}
}

TEST_CASE_TEMPLATE("Math", F, float, double, long double) {
	testAllFuncs<F, -2.L>();
	testAllFuncs<F, -1.L>();
	testAllFuncs<F, -0.5L>();
	testAllFuncs<F, 0.L>();
	testAllFuncs<F, 0.5L>();
	testAllFuncs<F, 1.L>();
	testAllFuncs<F, 2.L>();
}

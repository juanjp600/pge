#ifndef PGE_INTERPOLATOR_H_INCLUDED
#define PGE_INTERPOLATOR_H_INCLUDED

#include <cmath>

#include <PGE/Types/Concepts.h>

namespace PGE {

namespace Interpolator {
	template <Arithmetic T>
	constexpr const T lerp(const T& from, const T& to, float lerp) {
		return from * (T(1) - lerp) + to * lerp;
	}

	template <Arithmetic T>
	inline const T cerp(const T& from, const T& to, float lerp) {
		float s = (T(1) - cos(lerp * Math::PI)) / 2.f;
		return lerp(from, to, s);
	}
}

}

#endif // PGE_INTERPOLATOR_H_INCLUDED

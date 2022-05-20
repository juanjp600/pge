#ifndef PGE_INTERPOLATOR_H_INCLUDED
#define PGE_INTERPOLATOR_H_INCLUDED

#include "Math.h"

namespace PGE {

namespace Interpolator {
	template <typename T>
	concept Interpolatable = requires(const T & t, float f) {
		{ t + t } -> std::convertible_to<T>;
		{ t - t } -> std::convertible_to<T>;
		{ f * t } -> std::convertible_to<T>;
	};

	template <Interpolatable T>
	constexpr T lerp(const T& from, const T& to, float interp) {
		return from + interp * (to - from);
	}

	template <Interpolatable T>
	constexpr T cerp(const T& from, const T& to, float interp) {
		float s = (1.f - Math::cos(interp * Math::PI)) / 2.f;
		return lerp(from, to, s);
	}
}

}

#endif // PGE_INTERPOLATOR_H_INCLUDED

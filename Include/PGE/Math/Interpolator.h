#ifndef PGE_INTERPOLATOR_H_INCLUDED
#define PGE_INTERPOLATOR_H_INCLUDED

namespace PGE {

namespace Interpolator {
	template <class T>
	T lerp(const T& from, const T& to, float lerp) {
		return from * (1.f - lerp) + to * lerp;
	}

	template <class T>
	T cerp(const T& from, const T& to, float lerp) {
		float s = (1.f - std::cos(lerp * Math::PI)) / 2.f;
		return lerp(from, to, s);
	}
}

}

#endif // PGE_INTERPOLATOR_H_INCLUDED

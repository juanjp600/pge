#include <PGE/String/String.h>

#include <PGE/Math/Vector.h>

using namespace PGE;

template <typename T, auto T::*first, auto T::*... args>
static const String concat(const T& t) {
	return "(" + String::from(t.*first) +  ((", " + String::from(t.*args)) + ...) + ")";
}

template <> const String String::from(const Vector2f& vec) {
	return concat<Vector2f, &Vector2f::x, &Vector2f::y>(vec);
}

template <> const String String::from(const Vector3f& vec) {
	return concat<Vector3f, &Vector3f::x, &Vector3f::y, &Vector3f::z>(vec);
}

template <> const String String::from(const Vector4f& vec) {
	return concat<Vector4f, &Vector4f::x, &Vector4f::y, &Vector4f::z, &Vector4f::w>(vec);
}

template <> const String String::from(const Vector2i& vec) {
	return concat<Vector2i, &Vector2i::x, &Vector2i::y>(vec);
}

#ifndef PGE_FLAGENUM_H_INCLUDED
#define PGE_FLAGENUM_H_INCLUDED

#include "Concepts.h"

namespace PGE {

template <Enum E>
struct IsFlagEnum : Meta, std::false_type { };

template <typename E>
concept FlagEnum = Enum<E> && IsFlagEnum<E>::value;

}

template <PGE::FlagEnum E>
constexpr bool operator!(E e) { return !(typename std::underlying_type<E>::type)e; }
template <PGE::FlagEnum E>
constexpr E operator~(E e) { return (E)~(std::underlying_type_t<E>)e; }
template <PGE::FlagEnum E>
constexpr E operator&(E lhs, E rhs) {
	using UnderlyingType = std::underlying_type<E>::type;
	return (E)((UnderlyingType)lhs & (UnderlyingType)rhs);
}
template <PGE::FlagEnum E>
constexpr E operator|(E lhs, E rhs) {
	using UnderlyingType = std::underlying_type<E>::type;
	return (E)((UnderlyingType)lhs | (UnderlyingType)rhs);
}
template <PGE::FlagEnum E>
constexpr E operator^(E lhs, E rhs) {
	using UnderlyingType = std::underlying_type<E>::type;
	return (E)((UnderlyingType)lhs ^ (UnderlyingType)rhs);
}
template <PGE::FlagEnum E> constexpr void operator&=(E& lhs, E rhs) { lhs = lhs & rhs; }
template <PGE::FlagEnum E> constexpr void operator|=(E& lhs, E rhs) { lhs = lhs | rhs; }
template <PGE::FlagEnum E> constexpr void operator^=(E& lhs, E rhs) { lhs = lhs ^ rhs; }

#endif // PGE_FLAGENUM_H_INCLUDED

#ifndef PGE_FLAGENUM_H_INCLUDED
#define PGE_FLAGENUM_H_INCLUDED

#include "Concepts.h"

namespace PGE {

template <Enum E>
class FlagEnum {
	public:
		using UnderlyingType = std::underlying_type<E>::type;

	private:
		E value;
	
		FlagEnum(UnderlyingType val) : value((E)val) { }

	public:
		FlagEnum() = default;
		FlagEnum(E e) : value(e) { }

		operator bool() const { return (bool)value; }

		explicit operator UnderlyingType() const { return value; }

		FlagEnum operator~() const { return ~(UnderlyingType)value; }

		FlagEnum operator&(FlagEnum other) const { return (UnderlyingType)value & (UnderlyingType)other.value; }
		FlagEnum operator|(FlagEnum other) const { return (UnderlyingType)value | (UnderlyingType)other.value; }
		FlagEnum operator^(FlagEnum other) const { return (UnderlyingType)value ^ (UnderlyingType)other.value; }

		void operator&=(FlagEnum other) { (UnderlyingType&)value &= (UnderlyingType)other.value; }
		void operator|=(FlagEnum other) { (UnderlyingType&)value |= (UnderlyingType)other.value; }
		void operator^=(FlagEnum other) { (UnderlyingType&)value ^= (UnderlyingType)other.value; }

		bool operator==(const FlagEnum&) const = default;
};

}

#endif // PGE_FLAGENUM_H_INCLUDED

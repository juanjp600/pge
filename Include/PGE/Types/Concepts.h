#ifndef PGE_CONCEPTS_H_INCLUDED
#define PGE_CONCEPTS_H_INCLUDED

#include <type_traits>
#include <ranges>

namespace PGE {

template <typename R, typename T>
concept Enumerable = std::ranges::range<R>
			      && std::same_as<std::ranges::range_value_t<R>, T>;

template <typename E>
concept Enum = std::is_enum<E>::value;

#ifdef _MSC_VER
#pragma warning(disable: 4624)
#endif
struct Meta { Meta() = delete; ~Meta() = delete; };

struct False : Meta { static constexpr bool value = false; };
struct True : Meta { static constexpr bool value = true; };

template <template <typename...> typename ToNegate>
struct Negator : Meta {
	template <typename... Args>
	using Type = std::negation<ToNegate<Args...>>;
};

template <typename T>
concept Arithmetic = std::is_arithmetic<T>::value;

template <typename T, template <typename, typename> typename Checker, typename... CheckTypes>
concept Any = (Checker<T, CheckTypes>::value || ...);

template <typename T, template <typename, typename> typename Checker, typename... CheckTypes>
concept All = (Checker<T, CheckTypes>::value && ...);

template <typename T, typename... CheckTypes>
concept OneOf = Any<T, std::is_same, CheckTypes...>;

template <typename T, typename... CheckTypes>
concept AllExcept = Any<T, Negator<std::is_same>::Type, CheckTypes...>;

}

#endif // PGE_CONCEPTS_H_INCLUDED

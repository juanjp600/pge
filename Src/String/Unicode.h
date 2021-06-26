#ifndef PGE_UNICODE_H_INCLUDED
#define PGE_UNICODE_H_INCLUDED

#include <unordered_map>

#include <PGE/String/String.h>

namespace PGE {

namespace Unicode {
	extern const std::unordered_map<char32_t, char32_t> FOLDING;
	extern const std::unordered_map<char32_t, std::vector<char32_t>> MULTI_FOLDING;
	extern const std::unordered_map<char32_t, char32_t> UP;
	extern const std::unordered_map<char32_t, std::vector<char32_t>> MULTI_UP;
	extern const std::unordered_map<char32_t, char32_t> DOWN;
	extern const std::unordered_map<char32_t, std::vector<char32_t>> MULTI_DOWN;
}

}

#endif // PGE_UNICODE_H_INCLUDED

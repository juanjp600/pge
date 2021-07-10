#ifndef PGE_UNICODE_H_INCLUDED
#define PGE_UNICODE_H_INCLUDED

#include <unordered_map>

#include <PGE/String/String.h>

namespace PGE {

namespace Unicode {
	extern const std::unordered_map<char16, char16> FOLDING;
	extern const std::unordered_map<char16, std::vector<char16>> MULTI_FOLDING;
	extern const std::unordered_map<char16, char16> UP;
	extern const std::unordered_map<char16, std::vector<char16>> MULTI_UP;
	extern const std::unordered_map<char16, char16> DOWN;
	extern const std::unordered_map<char16, std::vector<char16>> MULTI_DOWN;
}

}

#endif // PGE_UNICODE_H_INCLUDED

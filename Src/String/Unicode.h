#ifndef PGE_UNICODE_H_INCLUDED
#define PGE_UNICODE_H_INCLUDED

#include <unordered_map>

#include <PGE/String/String.h>

namespace PGE {

namespace Unicode {
	extern const std::unordered_map<wchar, wchar> FOLDING;
	extern const std::unordered_map<wchar, std::vector<wchar>> MULTI_FOLDING;
	extern const std::unordered_map<wchar, wchar> UP;
	extern const std::unordered_map<wchar, std::vector<wchar>> MULTI_UP;
	extern const std::unordered_map<wchar, wchar> DOWN;
	extern const std::unordered_map<wchar, std::vector<wchar>> MULTI_DOWN;
}

}

#endif // PGE_UNICODE_H_INCLUDED

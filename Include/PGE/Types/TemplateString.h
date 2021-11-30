#ifndef PGE_TEMPLATESTRING_H_INCLUDED
#define PGE_TEMPLATESTRING_H_INCLUDED

#include <algorithm>

namespace PGE {

template <size_t N>
struct TemplateString {
	public:
		char cstr[N];
		  
		consteval TemplateString(const char(&cstri)[N]) { std::copy_n(cstri, N, cstr); }

		constexpr operator const char*() const { return cstr; }
};

}

#endif // PGE_TEMPLATESTRING_H_INCLUDED

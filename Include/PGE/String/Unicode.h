#ifndef PGE_UNICODE_H_INCLUDED
#define PGE_UNICODE_H_INCLUDED

#include <PGE/Types/Types.h>

namespace PGE {

namespace Unicode {
	// S and Cc.
	bool isSpace(char16 ch);
	static bool isDigit(char16 ch) { return ch >= L'0' && ch <= L'9'; }
}

}

#endif // PGE_UNICODE_H_INCLUDED
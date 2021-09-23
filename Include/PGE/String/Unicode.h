#ifndef PGE_UNICODE_H_INCLUDED
#define PGE_UNICODE_H_INCLUDED

#include <PGE/Types/Types.h>

namespace PGE {

namespace Unicode {
	// S and Cc.
	bool isSpace(char16 ch);
	bool isDigit(char16 ch);
}

}

#endif // PGE_UNICODE_H_INCLUDED
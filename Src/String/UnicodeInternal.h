#ifndef PGE_UNICODEINTERNAL_H_INCLUDED
#define PGE_UNICODEINTERNAL_H_INCLUDED

#include <PGE/String/String.h>
#include <PGE/Types/CircularArray.h>

namespace PGE {

namespace Unicode {
	void fold(CircularArray<char16>& queue, char16 ch);
	void up(String& str, char16 ch);
	void down(String& str, char16 ch);
}

}

#endif // PGE_UNICODEINTERNAL_H_INCLUDED

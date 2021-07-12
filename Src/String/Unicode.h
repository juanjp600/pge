#ifndef PGE_UNICODE_H_INCLUDED
#define PGE_UNICODE_H_INCLUDED

#include <queue>

#include <PGE/String/String.h>

namespace PGE {

namespace Unicode {
	void fold(std::queue<char16>& queue, char16 ch);
	void up(String& str, char16 ch);
	void down(String& str, char16 ch);
}

}

#endif // PGE_UNICODE_H_INCLUDED

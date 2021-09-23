#include <PGE/String/Unicode.h>

using namespace PGE;

bool Unicode::isDigit(char16 ch) {
	return ch >= L'0' && ch <= L'9';
}

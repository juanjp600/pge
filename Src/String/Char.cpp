#include <String/Char.h>

using namespace PGE;

wchar_t Char::fold(wchar ch) {
	auto it = folding.find(ch);
	if (it != folding.end()) {
		return it->second;
	}
	return ch;
}

bool Char::equal(wchar a, wchar b) {
	return fold(a) == fold(b);
}

wchar Char::toUpper(wchar ch) {
	auto it = up.find(ch);
	if (it != up.end()) {
		return it->second;
	}
	return ch;
}

wchar Char::toLower(wchar ch) {
	auto it = down.find(ch);
	if (it != down.end()) {
		return it->second;
	}
	return ch;
}

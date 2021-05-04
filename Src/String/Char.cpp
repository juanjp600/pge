#include <String/Char.h>

#include <Exception/Exception.h>

using namespace PGE;



wchar Char::fold(wchar ch) {
	auto it = folding.find(ch);
	if (it != folding.end()) {
		return it->second;
	}
	return ch;
}

std::vector<wchar>& Char::multiFold(wchar ch) {
	// TODO: Assert.
	return multiFolding.find(ch)->second;
}

Char::Equality Char::equal(wchar a, wchar b) {
	PGE_ASSERT(a != L'\uFFFF' && b != L'\uFFFF', "Invalid characters");
	wchar newA = fold(a);
	if (newA == L'\uFFFF') {
		return Equality::MULTI_A;
	}

	wchar newB = fold(b);
	if (newB == L'\uFFFF') {
		return Equality::MULTI_B;
	}
	return newA == newB ? Equality::EQUAL : Equality::DIFFERENT;
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

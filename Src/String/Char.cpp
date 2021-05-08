#include "Char.h"

#include <Exception/Exception.h>

using namespace PGE;

void Char::foldInto(wchar ch, std::queue<wchar>& deq) {
	auto it = folding.find(ch);
	if (it == folding.end()) {
		deq.push(ch);
		return;
	}

	wchar folded = it->second;
	if (folded == L'\uFFFF') {
		std::vector<wchar>& addChars = multiFolding.find(ch)->second;
		for (wchar add : addChars) {
			deq.push(add);
		}
		return;
	}

	deq.push(folded);
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

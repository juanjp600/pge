#include "Char.h"

#include <Exception/Exception.h>

using namespace PGE;

void Char::foldInto(wchar ch, std::deque<wchar>& deq) {
	auto it = folding.find(ch);
	if (it == folding.end()) {
		deq.push_front(ch);
	}
	wchar folded = it->second;
	if (folded == L'\uFFFF') {
		std::vector<wchar>& addChars = multiFolding.find(ch)->second;
		deq.insert(deq.begin(), addChars.begin(), addChars.end());
	} else {
		deq.push_front(folded);
	}
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

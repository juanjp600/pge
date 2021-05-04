#ifndef PGE_CHAR_H_DEFINED
#define PGE_CHAR_H_DEFINED

#include <unordered_map>
#include <deque>

#include <String/String.h>

namespace PGE {

class Char {
	public:
		static void foldInto(wchar ch, std::deque<wchar>& deq);
		static wchar toLower(wchar ch);
		static wchar toUpper(wchar ch);

	private:
		Char() = delete;

		static std::unordered_map<wchar, wchar> folding;
		static std::unordered_map<wchar, std::vector<wchar>> multiFolding;
		static std::unordered_map<wchar, wchar> up;
		static std::unordered_map<wchar, wchar> down;
};

}

#endif // PGE_CHAR_H_DEFINED

#ifndef PGE_CHAR_H_DEFINED
#define PGE_CHAR_H_DEFINED

#include <unordered_map>

#include <String/String.h>

namespace PGE {

class Char {
	public:
		static bool equal(wchar a, wchar b);
		static wchar toLower(wchar ch);
		static wchar toUpper(wchar ch);

	private:
		Char() = delete;

		static std::unordered_map<wchar, wchar> folding;
		static std::unordered_map<wchar, wchar> up;
		static std::unordered_map<wchar, wchar> down;
		static wchar_t fold(wchar ch);
};

}

#endif // PGE_CHAR_H_DEFINED

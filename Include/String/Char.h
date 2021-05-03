#ifndef PGE_CHAR_H_DEFINED
#define PGE_CHAR_H_DEFINED

namespace PGE {

class Char {
	public:
		static wchar_t toLower(wchar_t in);
		static wchar_t toUpper(wchar_t in);

	private:
		Char() = delete;
};

}

#endif // PGE_CHAR_H_DEFINED

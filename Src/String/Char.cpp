#include <String/Char.h>

#include <unordered_map>

using namespace PGE;

struct Maps {
	Maps(std::vector<std::tuple<wchar_t, wchar_t>> chars) {
		for (auto pair : chars) {
			down.emplace(std::get<1>(pair), std::get<0>(pair));
			up.emplace(std::get<0>(pair), std::get<1>(pair));
		}
	}

	std::unordered_map<wchar_t, wchar_t> down;
	std::unordered_map<wchar_t, wchar_t> up;
};

// TODO: Automatically generate this.
static Maps maps = Maps({
	// Basic
	{ L'a', L'A' },
	{ L'b', L'B' },
	{ L'c', L'C' },
	{ L'd', L'D' },
	{ L'e', L'E' },
	{ L'f', L'F' },
	{ L'g', L'G' },
	{ L'h', L'H' },
	{ L'i', L'I' },
	{ L'j', L'J' },
	{ L'k', L'K' },
	{ L'l', L'L' },
	{ L'm', L'M' },
	{ L'n', L'N' },
	{ L'o', L'O' },
	{ L'p', L'P' },
	{ L'q', L'Q' },
	{ L'r', L'R' },
	{ L's', L'S' },
	{ L't', L'T' },
	{ L'u', L'U' },
	{ L'v', L'V' },
	{ L'w', L'W' },
	{ L'x', L'X' },
	{ L'y', L'Y' },
	{ L'z', L'Z' },

	// Latin-1
	{ L'à', L'À' },
	{ L'á', L'Á' },
	{ L'â', L'Â' },
	{ L'ã', L'Ã' },
	{ L'ä', L'Ä' },
	{ L'å', L'Å' },
	{ L'æ', L'Æ' },
	{ L'ç', L'Ç' },
	{ L'è', L'È' },
	{ L'é', L'É' },
	{ L'ê', L'Ê' },
	{ L'ë', L'Ë' },
	{ L'ì', L'Ì' },
	{ L'í', L'Í' },
	{ L'î', L'Î' },
	{ L'ï', L'Ï' },
	{ L'ð', L'Ð' },
	{ L'ñ', L'Ñ' },
	{ L'ò', L'Ò' },
	{ L'ó', L'Ó' },
	{ L'ô', L'Ô' },
	{ L'õ', L'Õ' },
	{ L'ö', L'Ö' },
	{ L'ø', L'Ø' },
	{ L'ù', L'Ù' },
	{ L'ú', L'Ú' },
	{ L'û', L'Û' },
	{ L'ü', L'Ü' },
	{ L'ý', L'Ý' },
	{ L'þ', L'Þ' },
	{ L'ÿ', L'\u0178' },
});

wchar_t Char::toLower(wchar_t in) {
	auto it = maps.down.find(in);
	if (it != maps.down.end()) {
		return it->second;
	}
	return in;
}

wchar_t Char::toUpper(wchar_t in) {
	auto it = maps.up.find(in);
	if (it != maps.up.end()) {
		return it->second;
	}
	return in;
}

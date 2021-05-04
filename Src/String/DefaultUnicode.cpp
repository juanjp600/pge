#if not __has_include("Unicode.cpp")

#include <String/Char.h>

using namespace PGE;

std::unordered_map<wchar, wchar> Char::folding;
std::unordered_map<wchar, wchar> Char::up;
std::unordered_map<wchar, wchar> Char::down;

#endif // not __has_include("Unicode.cpp")

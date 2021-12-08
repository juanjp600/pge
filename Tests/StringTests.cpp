#include "Util.h"

#include <iostream>

#include <PGE/String/String.h>
#include <PGE/Exception/Exception.h>

using namespace PGE;

TEST_SUITE("Strings") {

static void checkLength(const String& str, int byteLength, int length) {
	CHECK(str.byteLength() == byteLength);
	CHECK(str.length() == length);
}

TEST_CASE("Lengths") {
	checkLength("", 0, 0);
	checkLength("A", 1, 1);
	checkLength(L"Ä", 2, 1);
	checkLength(u8"ÜÖÄ", 6, 3);
	checkLength(L"ÄAA", 4, 3);
	checkLength(u8"AÄA", 4, 3);
	checkLength(L"AAÄ", 4, 3);
	checkLength(u8"Hello, this is a very long string, it is very long, yeah, very long, like very long.\n"
		u8"Ok, I'll also make it include special characters like Ä and Ü and idfk \u2764 (it's a heart)", 176, 172);
}

TEST_CASE("Basic iterator tests") {
	String a;
	SUBCASE_PARAMETERIZE((a, "A", u8"Ä", L"\u2764"));

	CHECK(a.begin() == a.rbegin());
	CHECK(a.begin() + 1 == a.end());
	CHECK(a.begin() == a.end() - 1);
	CHECK(a.rbegin() + 1 == a.rend());
	CHECK(a.rbegin() == a.rend() - 1);

	String e;
	CHECK(e.begin() == e.end());
	CHECK(e.rbegin() == e.rend());

	CHECK(e.begin() != e.rbegin());
	CHECK(e.end() != e.rend());
	CHECK(e.rbegin() != e.end());
	CHECK(e.begin() != e.rend());

	String o;
	CHECK(e.begin() != o.begin());
	CHECK(e.end() != o.end());
	CHECK(e.rbegin() != o.rbegin());
	CHECK(e.rend() != o.rend());
}

TEST_CASE("Repeat") {
	String a; String sep;
	SUBCASE_PARAMETERIZE(
		(a, "A", u8"Ä", "ASDFG", u8"ÄADß", u8"Öaaa", u8"oooÄ"),
		(sep, "", ", ", "A", u8"ÖÜÖ")
	);

	CHECK(a.repeat(0) == String());
	CHECK(a.repeat(1) == a);
	
}

TEST_CASE("Repeat explicit") {
	String a = "A";
	CHECK(a.repeat(5) == "AAAAA");
	CHECK(a.repeat(5, ", ") == "A, A, A, A, A");

	String u = u8"Ä";
	CHECK(u.repeat(5) == u8"ÄÄÄÄÄ");
	CHECK(u.repeat(5, ", ") == u8"Ä, Ä, Ä, Ä, Ä");

	CHECK_THROWS_PGE(a.repeat(-1));
	CHECK_THROWS_PGE(u.repeat(-1));
}

TEST_CASE("Reverse explicit") {
	CHECK(String().reverse() == "");
	String a = L"gÄnÄg";
	CHECK(a.reverse() == a);
	CHECK(String(L"Ä").reverse() == L"Ä");
	CHECK(String("reverse").reverse() == "esrever");
	CHECK(String(L"ÄBC").reverse() == L"CBÄ");
}

TEST_CASE("Split explicit") {
	String e;
	CHECK(e.split("", true).empty());
	CHECK(e.split("", false) == std::vector<String>(2, ""));
	CHECK(e.split("asd", false) == std::vector<String>{ "" });

	String a = "pulseyesgun";
	CHECK(a.split("yes", true) == std::vector<String>{ "pulse", "gun" });
	CHECK(a.split("pulse", false) == std::vector<String>{ "", "yesgun" });
	CHECK(a.split("pulse", true) == std::vector<String>{ "yesgun" });
	CHECK(a.split("gun", false) == std::vector<String>{ "pulseyes", "" });
	CHECK(a.split("gun", true) == std::vector<String>{ "pulseyes" });
	CHECK(a.split("pulseyesgun", false) == std::vector<String>(2));
	CHECK(a.split("pulseyesgun", true).empty());
	CHECK(a.split("", false).size() == a.length() + 2);
	CHECK(a.split("", true).size() == a.length());

	String u = L"ÖöäÄäüÜ";
	CHECK(u.split(u8"Ö", false) == std::vector<String>{ "", L"öäÄäüÜ" });
	CHECK(u.split(u8"Ä", false) == std::vector<String>{ L"Ööä", L"äüÜ" });
	CHECK(u.split(u8"Ü", false) == std::vector<String>{ L"ÖöäÄäü", "" });
	CHECK(u.split("", false).size() == u.length() + 2);
	CHECK(u.split("", true).size() == u.length());

	CHECK(String(L"ßßßßßß").split(L"ßß", false) == std::vector<String>(4, ""));
	CHECK(String(L"ßßßßßßß").split(L"ßß", false) == std::vector<String>{ "", "", "", L"ß" });
	CHECK(String(L"ßßß").split(L"ß", false) == std::vector<String>(4, ""));
}

TEST_CASE("Multiply split join same") {
	String a; String sep; int i;
	SUBCASE_PARAMETERIZE(
		(a, "A", L"ÄäÄ"),
		(sep, ",", "", "   ", "ö"),
		(i, 10, 1, 0, 100)
	);

	a = a.repeat(i, sep);
	CHECK(a == String::join(a.split(sep, true), sep));
}

TEST_CASE("Comparison strcmp compatibility") {
	String a; String b;
	// No Unicode allowed!
	#define COMPARISON_CASES "A", "B", "C", "ASD", "FFF", "1", "9", "109", "A1", "aA1", "2FO", "222222222", "asddsad"
	SUBCASE_PARAMETERIZE(
		(a, COMPARISON_CASES),
		(b, COMPARISON_CASES)
	);
	#undef COMPARISON_CASES
	CHECK(a.compare(b) == (strcmp(a.cstr(), b.cstr()) <=> 0));
}

TEST_CASE("Trim explicit") {
	CHECK(String("          trollge    ").trim() == "trollge");
	CHECK(String("                ").trim() == "");
	CHECK(String().trim() == "");
	CHECK(String(L"    ÄßC ").trim() == L"ÄßC");
	CHECK(String(u8"    ÄßC ").trim() == u8"ÄßC");
	CHECK(String(u8"\u2009ÄßC \u00A0 \f \n").trim() == u8"ÄßC");
}

TEST_CASE("RegEx explicit") {
	String regexfull("[A-Za-z]+");
	CHECK(String("pulsegun").regexMatch(regexfull) == "pulsegun");
	CHECK(String().regexMatch(regexfull).isEmpty());

	String b = u8"Ä";
	CHECK(b.regexMatch(u8"[Ä]") == u8"Ä");
	CHECK(b.regexMatch(u8"[ä]").isEmpty());
}

TEST_CASE("Uppercase explicit") {
	CHECK(String("juano").toUpper() == "JUANO");
	CHECK(String(u8"jüano").toUpper() == u8"JÜANO");
	CHECK(String(u8"jü !@#$%^\u2764&*() ano\u2764").toUpper() == u8"JÜ !@#$%^\u2764&*() ANO\u2764");
}

TEST_CASE("Replace explicit") {
	String a = "pulsegoop";
	CHECK(a.replace("goop", "gun") == "pulsegun");
	CHECK(a.replace("p", "gun") == "gunulsegoogun");
	CHECK(a.replace("goopa", "not") == "pulsegoop");
	CHECK(a.replace("NOT", "") == "pulsegoop");
	CHECK(a.replace("pulsegoop", "") == "");
	CHECK(a.replace("goop", "") == "pulse");
	CHECK(a.replace(u8"\u2009", "") == "pulsegoop");
	CHECK(a.replace("goop", u8"\uFB05\uFB05\uFB05") == u8"pulse\uFB05\uFB05\uFB05");
	CHECK(String("pulse gu n").replace(" ", "") == "pulsegun");
	CHECK(String(u8"pülse gü nü").replace(" ", "") == u8"pülsegünü");
	CHECK(String(u8"p ü l s e  g ü  n ü").replace("  ", "") == u8"p ü l s eg ün ü");
	CHECK(String(L"ÄßD").replace("", "|") == L"|Ä|ß|D|");
	CHECK(String().replace("", "|") == "|");
}

TEST_CASE("Substring explicit") {
	String a = L"pulsegöop";
	CHECK(a.substr(5) == L"göop");
	CHECK(a.substr(9) == "");
	CHECK(String(L"ÖöäÄäüÜ").substr(3) == L"ÄäüÜ");

	CHECK(a.substr(0, 0) == "");
	CHECK(a.substr(0, 5) == "pulse");
	CHECK(a.substr(1, 2) == "ul");
	CHECK(a.substr(a.length(), 0) == "");
	CHECK(a.substr(0, a.length()) == a);
	CHECK(a.substr(a.begin(), a.end()) == a);
	CHECK(a.substr(a.findFirst(L"ö") + 1) == "op");

	CHECK(String().substr(0, 0) == "");
	
	CHECK_THROWS_PGE(a.substr(a.end(), a.begin()));
	CHECK_THROWS_PGE(a.substr(a.end(), a.end() - 1));
	CHECK_THROWS_PGE(a.substr(0, a.byteLength() + 1));
	CHECK_THROWS_PGE(a.substr(a.byteLength(), 1));
	CHECK_THROWS_PGE(a.substr(10));
}

}

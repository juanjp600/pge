#include "Util.h"

#include <iostream>

#include <PGE/String/String.h>
#include <PGE/Exception/Exception.h>

#define DOCTEST_STL_STRINGIFY_FLIP
#include <doctest/extensions/stl_stringifier.h>

using namespace PGE;

TEST_SUITE("Strings") {

static void checkLength(const String& str, int byteLength, int length) {
	CHECK(str.byteLength() == byteLength);
	CHECK(str.length() == length);
}

TEST_CASE("Lengths") {
	checkLength("", 0, 0);
	checkLength("A", 1, 1);
	checkLength(L"�", 2, 1);
	checkLength(u8"���", 6, 3);
	checkLength(L"�AA", 4, 3);
	checkLength(u8"A�A", 4, 3);
	checkLength(L"AA�", 4, 3);
	checkLength(u8"Hello, this is a very long string, it is very long, yeah, very long, like very long.\n"
		u8"Ok, I'll also make it include special characters like � and � and idfk \u2764 (it's a heart)", 176, 172);
}

TEST_CASE("Basic iterator tests") {
	String a;
	SUBCASE_PARAMETERIZE((a, "A", u8"�", L"\u2764"));

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
		(a, "A", u8"�", "ASDFG", u8"�AD�", u8"�aaa", u8"ooo�"),
		(sep, "", ", ", "A", u8"���")
	);

	CHECK(a.repeat(0) == String());
	CHECK(a.repeat(1) == a);
	
}

TEST_CASE("Repeat explicit") {
	String a = "A";
	CHECK(a.repeat(5) == "AAAAA");
	CHECK(a.repeat(5, ", ") == "A, A, A, A, A");

	String u = u8"�";
	CHECK(u.repeat(5) == u8"�����");
	CHECK(u.repeat(5, ", ") == u8"�, �, �, �, �");

	CHECK_THROWS_PGE(a.repeat(-1));
	CHECK_THROWS_PGE(u.repeat(-1));
}

TEST_CASE("Reverse explicit") {
	CHECK(String().reverse() == "");
	String a = L"g�n�g";
	CHECK(a.reverse() == a);
	CHECK(String(L"�").reverse() == L"�");
	CHECK(String("reverse").reverse() == "esrever");
	CHECK(String(L"�BC").reverse() == L"CB�");
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

	String u = L"�������";
	CHECK(u.split(u8"�", false) == std::vector<String>{ "", L"������" });
	CHECK(u.split(u8"�", false) == std::vector<String>{ L"���", L"���" });
	CHECK(u.split(u8"�", false) == std::vector<String>{ L"������", "" });
	CHECK(u.split("", false).size() == u.length() + 2);
	CHECK(u.split("", true).size() == u.length());

	CHECK(String(L"������").split(L"��", false) == std::vector<String>(4, ""));
	CHECK(String(L"�������").split(L"��", false) == std::vector<String>{ "", "", "", L"�" });
	CHECK(String(L"���").split(L"�", false) == std::vector<String>(4, ""));
}

TEST_CASE("Multiply split join same") {
	String a; String sep; int i;
	SUBCASE_PARAMETERIZE(
		(a, "A", L"���"),
		(sep, ",", "", "   ", "�"),
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
	CHECK(String(L"    ��C ").trim() == L"��C");
	CHECK(String(u8"    ��C ").trim() == u8"��C");
	CHECK(String(u8"\u2009��C \u00A0 \f \n").trim() == u8"��C");
}

TEST_CASE("RegEx explicit") {
	String regexfull("[A-Za-z]+");
	CHECK(String("pulsegun").regexMatch(regexfull) == "pulsegun");
	CHECK(String().regexMatch(regexfull).isEmpty());

	String b = u8"�";
	CHECK(b.regexMatch(u8"[�]") == u8"�");
	CHECK(b.regexMatch(u8"[�]").isEmpty());
}

TEST_CASE("Uppercase explicit") {
	CHECK(String("juano").toUpper() == "JUANO");
	CHECK(String(u8"j�ano").toUpper() == u8"J�ANO");
	CHECK(String(u8"j� !@#$%^\u2764&*() ano\u2764").toUpper() == u8"J� !@#$%^\u2764&*() ANO\u2764");
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
	CHECK(String(u8"p�lse g� n�").replace(" ", "") == u8"p�lseg�n�");
	CHECK(String(u8"p � l s e  g �  n �").replace("  ", "") == u8"p � l s eg �n �");
	CHECK(String(L"��D").replace("", "|") == L"|�|�|D|");
	CHECK(String().replace("", "|") == "|");
}

TEST_CASE("Substring explicit") {
	String a = L"pulseg�op";
	CHECK(a.substr(5) == L"g�op");
	CHECK(a.substr(9) == "");
	CHECK(String(L"�������").substr(3) == L"����");

	CHECK(a.substr(0, 0) == "");
	CHECK(a.substr(0, 5) == "pulse");
	CHECK(a.substr(1, 2) == "ul");
	CHECK(a.substr(a.length(), 0) == "");
	CHECK(a.substr(0, a.length()) == a);
	CHECK(a.substr(a.begin(), a.end()) == a);
	CHECK(a.substr(a.findFirst(L"�") + 1) == "op");

	CHECK(String().substr(0, 0) == "");
	
	CHECK_THROWS_PGE(a.substr(a.end(), a.begin()));
	CHECK_THROWS_PGE(a.substr(a.end(), a.end() - 1));
	CHECK_THROWS_PGE(a.substr(0, a.byteLength() + 1));
	CHECK_THROWS_PGE(a.substr(a.byteLength(), 1));
	CHECK_THROWS_PGE(a.substr(10));
}

}

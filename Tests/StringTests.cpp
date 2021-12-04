#include <doctest/doctest.h>

#include <PGE/String/String.h>

using namespace PGE;

TEST_SUITE("Strings") {

TEST_CASE("Very basic tests") {
	String empty;
	CHECK_EQ(empty.byteLength(), 0);
	CHECK_EQ(empty.length(), 0);
}

TEST_CASE("Multiplication") {
	String a = "A";
	CHECK_EQ(a.multiply(0), String());
	CHECK_EQ(a.multiply(1), a);
	CHECK_EQ(a.multiply(5), "AAAAA");
}

TEST_CASE("Multiplication with seperator") {
	String a = "A";
	String sep = ", ";
	CHECK_EQ(a.multiply(0, sep), String());
	CHECK_EQ(a.multiply(1, sep), a);
	CHECK_EQ(a.multiply(5, sep), "A, A, A, A, A");
}

}

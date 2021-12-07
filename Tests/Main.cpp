#define DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

TEST_CASE("Test test framework") {
	CHECK(1 == 1);
}

int main(int argc, char** argv) {
	doctest::Context context(argc, argv);

	std::ostream out(std::cout.rdbuf());
	class : public std::streambuf {
	public:
		int overflow(int c) { return c; }
	} nulBuf;
	std::cout.set_rdbuf(&nulBuf);

	context.setCout(&out);

	return context.run();
}

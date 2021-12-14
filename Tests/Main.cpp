#define DOCTEST_CONFIG_IMPLEMENT
#include "Util.h"

#include <PGE/Exception/Exception.h>

using namespace PGE;

REGISTER_EXCEPTION_TRANSLATOR(const Exception& e) {
	return e.what().cstr();
}

TEST_CASE("Test test framework") {
	CHECK(1 == 1);
}

int main(int argc, char** argv) {
	doctest::Context context(argc, argv);

	class : public std::streambuf {
	public:
		int overflow(int c) { return c; }
	} nulBuf;
	std::cout.set_rdbuf(&nulBuf);

	context.setCout(&pgeCout);

	return context.run();
}

#include <PGE/Exception/Exception.h>

#ifdef DEBUG
#include <iostream>
#endif

using namespace PGE;

// TODO: Add utility to differentiate between valid and invalid exceptions.
Exception::Exception() noexcept : info(">>> INVALID EXCEPTION <<<") { }

Exception::Exception(const String& file, int line, const String& extra) noexcept
    : info(file + ":" + String::fromInt(line) + (extra == "" ? "" : '\n' + extra)) {
#ifdef DEBUG
    std::cout << what() << std::endl;
#endif
}

const String& Exception::what() const noexcept {
    return info;
}

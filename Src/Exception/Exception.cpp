#include <PGE/Exception/Exception.h>

#ifdef DEBUG
#include <iostream>
#endif

using namespace PGE;

static const inline String INVALID_EX = ">>> INVALID EXCEPTION <<<";

Exception::Exception(const String& file, int line, const String& extra) noexcept {
    String ex = file + ':' + String::from(line);
    if (!extra.isEmpty()) {
        ex += '\n' + extra;
    }
    info = ex;
#ifdef DEBUG
    std::cout << what() << std::endl;
#endif
}

const String& Exception::what() const noexcept {
    return info.has_value() ? info.value() : INVALID_EX;
}

bool Exception::isValid() const noexcept {
    return info.has_value();
}

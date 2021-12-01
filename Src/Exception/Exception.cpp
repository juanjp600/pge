#include <PGE/Exception/Exception.h>

#ifdef DEBUG
#include <iostream>
#undef assert
#endif

using namespace PGE;

static const inline String INVALID_EX = ">>> INVALID EXCEPTION <<<";

Exception::Exception(const String& info, const std::source_location& location) noexcept {
    String ex = location.file_name() + '(' + String::from(location.line()) +
        ',' + String::from(location.column()) + "): "+ location.function_name();
    if (!info.isEmpty()) {
        ex += '\n' + info;
    }
    this->info = ex;
#ifdef DEBUG
    std::cout << what() << std::endl;
#endif
}

const String& Exception::what() const noexcept {
    return isValid() ? info.value() : INVALID_EX;
}

bool Exception::isValid() const noexcept {
    return info.has_value();
}

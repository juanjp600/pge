#include <Exception/Exception.h>

#ifdef DEBUG
#include <iostream>
#endif

using namespace PGE;

Exception::Exception() {
    info = ">>> INVALID EXCEPTION <<<";
}

Exception::Exception(const char* file, int line, const String& extra) {
    info = file;
    info += ':';
    info += String::fromInt(line);
    if (extra != "") {
        info += '\n';
        info += extra;
    }
#ifdef DEBUG
    std::cout << what() << std::endl;
#endif
}

const String& Exception::what() const {
    return info;
}

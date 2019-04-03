#include "Exception.h"

using namespace PGE;

Exception::Exception(String src,String detail) {
    source = src;
    details = detail;
}

String Exception::getSource() const {
    return source;
}

String Exception::getDetails() const {
    return details;
}

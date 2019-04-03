#include "Exception.h"

using namespace PGE;

Exception::Exception() {
    source = "N/A";
    details = "Improperly constructed exception";
}

Exception::Exception(const Exception& ex) {
    source = ex.getSource();
    details = ex.getDetails();
}

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

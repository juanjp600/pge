#ifndef PGEINTERNAL_EXCEPTION_H_INCLUDED
#define PGEINTERNAL_EXCEPTION_H_INCLUDED

#include <exception>
#include <Misc/String.h>

namespace PGE {

class Exception {
    public:
        Exception();
        Exception(const Exception& ex);
        Exception(String src,String detail);
        String getSource() const;
        String getDetails() const;
    private:
        String source;
        String details;
};

}

#endif

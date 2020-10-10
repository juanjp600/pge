#ifndef PGE_EXCEPTION_H_INCLUDED
#define PGE_EXCEPTION_H_INCLUDED

#include <Misc/String.h>

namespace PGE {

class Exception {
    public:
        Exception();
        Exception(const Exception& ex);
        Exception(String src, String detail);
        String getSource() const;
        String getDetails() const;

    private:
        String source;
        String details;
};

}

#endif // PGE_EXCEPTION_H_INCLUDED

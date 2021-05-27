#ifndef PGE_EXCEPTION_H_INCLUDED
#define PGE_EXCEPTION_H_INCLUDED

#include <PGE/String/String.h>

namespace PGE {

class Exception {
    public:
        // Invalid exception.
        Exception();

        // Never use this directly.
        Exception(const char* file, int line, const String& extra);

        const String& what() const;

    private:
        String info;
};

}

#define PGE_CREATE_EX(INFO) PGE::Exception(__FILE__, __LINE__, INFO)

#define PGE_ASSERT(COND, INFO) if (!(COND)) throw PGE_CREATE_EX(INFO)

#endif // PGE_EXCEPTION_H_INCLUDED

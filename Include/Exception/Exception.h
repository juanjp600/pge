#ifndef PGE_EXCEPTION_H_INCLUDED
#define PGE_EXCEPTION_H_INCLUDED

#include <Misc/String.h>

namespace PGE {

class Exception {
    public:
        // TODO: Remove.
        Exception(const String&, const String&) { }
        Exception() { }
        //

        Exception(const char* file, int line, const String& extra = "");

        const String& what() const;

    private:
        String info;
};

}

#define __EX_THRW throw PGE::Exception(__FILE__, __LINE__)
#define __EX_THRW_INFO(INFO) throw PGE::Exception(__FILE__, __LINE__, INFO)

#define __PGE_ASSERT(COND) if (!COND) __EX_THRW

#endif // PGE_EXCEPTION_H_INCLUDED

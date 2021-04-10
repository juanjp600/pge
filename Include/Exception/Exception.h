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

#define __EX_THRW(E) if (!E) throw PGE::Exception(__FILE__, __LINE__)
#define __EX_THRW_I(E, I) if (!E) throw PGE::Exception(__FILE__, __LINE__, I) 

#ifdef EXCEPTION_CATCH_HIGH
    #define EXCEPTION_CATCH_MID
    #define ASSERT_H(E) __EX_THRW(E)
    #define ASSERT_H_I(E, I) __EX_THRW_I(E, I)
#else
    #define ASSERT_H(E)
    #define ASSERT_H_I(E, I)
#endif

#ifdef EXCEPTION_CATCH_MID
    #define EXCEPTION_CATCH_LOW
    #define ASSERT_M(E) __EX_THRW(E)
    #define ASSERT_M_I(E, I) __EX_THRW_I(E, I)
#else
    #define ASSERT_M(E)
    #define ASSERT_M_I(E, I)
#endif

#ifdef EXCEPTION_CATCH_LOW
    #define ASSERT_L(E) __EX_THRW(E)
    #define ASSERT_L_I(E, I) __EX_THRW_I(E, I)
#else
    #define ASSERT_L(E)
    #define ASSERT_L_I(E, I)
#endif

}

#endif // PGE_EXCEPTION_H_INCLUDED

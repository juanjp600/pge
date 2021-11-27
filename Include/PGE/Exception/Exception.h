#ifndef PGE_EXCEPTION_H_INCLUDED
#define PGE_EXCEPTION_H_INCLUDED

#include <optional>

#include <PGE/String/String.h>

namespace PGE {

/// Singular exception class thrown by PGE.
/// @see #PGE::PGE_CREATE_EX, #PGE::PGE_ASSERT
class Exception {
    public:
        /// Invalid exception.
        /// Only ever use for storing exceptions by value.
        Exception() noexcept = default;

        /// DO NOT USE.
        /// @see #PGE_CREATE_EX, #PGE_ASSERT
        /// @deprecated Use associated macros.
        Exception(const String& file, int line, const String& extra) noexcept;

        /// Returns information about the exception, including the file and line it occured on and the user provided info.
        const String& what() const noexcept;

        /// Returns whether this exception is invalid (default constructed) or properly initialized.
        bool isValid() const noexcept;

    private:
        std::optional<String> info;
};

}

// TODO: No worky.
/// Creates an exception.
#define PGE_CREATE_EX(INFO) PGE::Exception(__FILE__, __LINE__, INFO)

/// Asserts that COND evaluates to true and throws an exception containing INFO otherwise.
#define PGE_ASSERT(COND, INFO) if (!(COND)) throw PGE_CREATE_EX(INFO)

#endif // PGE_EXCEPTION_H_INCLUDED

#ifndef PGE_EXCEPTION_H_INCLUDED
#define PGE_EXCEPTION_H_INCLUDED

#include <optional>
#include <source_location>

#include <PGE/String/String.h>

namespace PGE {

/// Singular exception class thrown by PGE.
class Exception {
    public:
        /// Invalid exception.
        /// Only ever use for storing exceptions by value.
        Exception() noexcept = default;

        /// Creates an exception.
        Exception(const String& info, const std::source_location& location = std::source_location::current()) noexcept;

        /// Returns information about the exception, including the file and line it occured on and the user provided info.
        const String& what() const noexcept;

        /// Returns whether this exception is invalid (default constructed) or properly initialized.
        bool isValid() const noexcept;

    private:
        std::optional<String> info;
};

// TODO: No worky.
/// Asserts that COND evaluates to true and throws an exception containing INFO otherwise.
#define PGE_ASSERT(COND, INFO) if (!(COND)) throw PGE::Exception(INFO)

#define PGE_ASSERT_AT(COND, INFO, WHERE) if (!(COND)) throw PGE::Exception(INFO, WHERE)

}

#endif // PGE_EXCEPTION_H_INCLUDED

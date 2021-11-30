#ifndef BUILDSHADERS_PARSER_H_INCLUDED
#define BUILDSHADERS_PARSER_H_INCLUDED

#include <PGE/Exception/Exception.h>
#include <PGE/String/String.h>
#include <PGE/String/Unicode.h>

#include <functional>

namespace Parser {
    bool isNotDigitOrSemicolon(PGE::char16 chr);
    bool isNotComma(PGE::char16 chr);
    bool isOpeningBrace(PGE::char16 chr);
    bool isClosingBrace(PGE::char16 chr);
    bool isBlockOpener(PGE::char16 chr);
    bool isBlockCloser(PGE::char16 chr);
    bool isIdentifierCharacter(PGE::char16 chr);

    template <std::input_or_output_iterator T>
        requires std::same_as<typename std::iterator_traits<T>::value_type, PGE::char16>
    void skip(T& it, const std::function<bool(PGE::char16)>& predicate) {
        while (predicate(*it)) {
            it++;
        }
    }

    void skipBlock(PGE::String::Iterator& it);
    void skipStatement(PGE::String::Iterator& it);

    void expectFixed(PGE::String::Iterator& it, PGE::char16 ch);
}

#endif // BUILDSHADERS_PARSER_H_INCLUDED

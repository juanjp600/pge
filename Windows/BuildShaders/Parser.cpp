#include "Parser.h"

bool Parser::isNotDigitOrSemicolon(PGE::char16 chr) {
    return !PGE::Unicode::isDigit(chr) && (chr != L';');
}

bool Parser::isNotComma(PGE::char16 chr) {
    return chr != ',';
}

bool Parser::isOpeningBrace(PGE::char16 chr) {
    return chr == '{';
}

bool Parser::isClosingBrace(PGE::char16 chr) {
    return chr == '}';
}

bool Parser::isBlockOpener(PGE::char16 chr) {
    return chr == '{' || chr == '(' || chr == '[';
}

bool Parser::isBlockCloser(PGE::char16 chr) {
    return chr == '}' || chr == ')' || chr == ']';
}

bool Parser::isIdentifierCharacter(PGE::char16 chr) {
    return !(PGE::Unicode::isSpace(chr) || (chr == ',') || (chr == ';') || (chr == ':') || isBlockOpener(chr) || isBlockCloser(chr));
}

void Parser::skipBlock(PGE::String::Iterator& it) {
    int nestDepth = 0;
    do {
        if (isBlockOpener(*it)) { nestDepth++; }
        else if (isBlockCloser(*it)) { nestDepth--; }
        it++;
    } while (nestDepth > 0);
}

void Parser::skipStatement(PGE::String::Iterator& it) {
    while (isIdentifierCharacter(*it)) {
        it++;
        if (isBlockOpener(*it)) { skipBlock(it); }
    }
}

void Parser::expectFixed(PGE::String::Iterator& it, PGE::char16 ch) {
    assert(*it == ch, PGE::String("Expected \"") + ch + "\", found \"" + *it + '"');
    it++;
}

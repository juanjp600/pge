#include "Parser.h"
#include "CompileResult.h"

using namespace PGE;

CompileResult::HlslStruct CompileResult::parseHlslStruct(const String& hlsl, const String& structName) {
    // struct 'VS_INPUT_NAME' {
    //     'TYPE' 'INPUT_NAME' : 'SEMANTIC_NAME''SEMANTIC_INDEX';
    //     'TYPE' 'INPUT_NAME' : 'SEMANTIC_NAME';
    //     'TYPE' 'INPUT_NAME' : 'SEMANTIC_NAME''SEMANTIC_INDEX';
    // }

    CompileResult::HlslStruct parsedStruct;
    parsedStruct.name = structName;

    String structDecl = "struct " + structName;
    String::Iterator before = hlsl.findFirst(structDecl) + structDecl.length();
    // Space before {
    Parser::skip(before, Unicode::isSpace);
    // {
    Parser::expectFixed(before, L'{');

    // Whitespace before first type
    Parser::skip(before, Unicode::isSpace);
    while (*before != L'}') {
        CompileResult::HlslStruct::Member member;

        String::Iterator after = before;
        // Type
        Parser::skip(after, std::not_fn(Unicode::isSpace));
        member.type = hlsl.substr(before, after);
        // Whitespace after type
        Parser::skip(after, Unicode::isSpace);

        before = after;
        // Member name
        Parser::skip(after, std::not_fn(Unicode::isSpace));
        member.name = hlsl.substr(before, after);

        before = after;
        // Whitespace before colon
        Parser::skip(before, Unicode::isSpace);
        Parser::expectFixed(before, L':');
        // Whitespace after colon
        Parser::skip(before, Unicode::isSpace);

        after = before;
        Parser::skip(after, Parser::isNotDigitOrSemicolon);

        member.dxSemanticName = hlsl.substr(before, after);
        if (*after == ';') {
            member.dxSemanticIndex = 0;
        }
        else {
            member.dxSemanticIndex = *after - '0';
            after++;
        }

        if (member.type.length() > 2 && member.type.substr(0, 2) != "//") {
            parsedStruct.members.push_back(member);
        }

        before = after;
        // Skip semicolon
        before++;

        // Whitespace before next type
        Parser::skip(before, Unicode::isSpace);
    }

    return parsedStruct;
}

const std::vector<CompileResult::Function> CompileResult::extractFunctions(const String& hlsl) {
    std::vector<Function> ret;
    String::Iterator closingPos = hlsl.findFirst(")");
    while (closingPos != hlsl.end()) {
        String::Iterator startBody = closingPos + 1;
        Parser::skip(startBody, Unicode::isSpace);
        if (*startBody == L'{') {
            // This might be a function!
            int depth = 0;
            String::Iterator argsBegin = closingPos;
            while (depth >= 0) {
                argsBegin--;
                if (*argsBegin == L')') {
                    depth++;
                } else if (*argsBegin == L'(') {
                    depth--;
                }
            }

            String::ReverseIterator nameEnd = argsBegin - 1;
            Parser::skip(nameEnd, Unicode::isSpace);
            String::ReverseIterator nameBegin = nameEnd;
            Parser::skip(nameBegin, Parser::isIdentifierCharacter);

            String name = hlsl.substr(nameBegin - 1, nameEnd - 1);
            if (name != "VS" && name != "PS") {
                String::ReverseIterator typeEnd = nameBegin + 1;
                Parser::skip(typeEnd, Unicode::isSpace);
                String::ReverseIterator typeStart = typeEnd;
                Parser::skip(typeStart, Parser::isIdentifierCharacter);

                if (typeStart != typeEnd) {
                    String::Iterator endBody = startBody;
                    Parser::skipBlock(endBody);

                    Function newFunc;
                    newFunc.name = name;
                    newFunc.func = hlsl.substr(typeStart - 1, endBody);
                    ret.emplace_back(newFunc);
                }
            }
        }

        closingPos = hlsl.findFirst(")", closingPos + 1);
    }
    return ret;
}

static const inline String CONST_SPECIFIERS = "static const ";

const std::vector<CompileResult::Constant> CompileResult::extractConstants(const String& hlsl) {
    std::vector<CompileResult::Constant> ret;
    String::Iterator constPos = hlsl.findFirst(CONST_SPECIFIERS);
    while (constPos != hlsl.end()) {
        ret.emplace_back();
        String::Iterator before; String::Iterator after;

        before = constPos + CONST_SPECIFIERS.length();
        Parser::skip(before, Unicode::isSpace);
        after = before;
        Parser::skip(after, Parser::isIdentifierCharacter);
        ret.back().type = hlsl.substr(before, after);

        Parser::skip(after, Unicode::isSpace);
        before = after;
        Parser::skip(after, Parser::isIdentifierCharacter);
        ret.back().name = hlsl.substr(before, after);

        Parser::skip(after, Unicode::isSpace);
        Parser::expectFixed(after, L'=');
        Parser::skip(after, Unicode::isSpace);
        before = after;
        Parser::skip(after, [](char16 ch) { return ch != ';'; });
        ret.back().value = hlsl.substr(before, after);

        constPos = hlsl.findFirst(CONST_SPECIFIERS, after);
    }
    return ret;
}

const std::vector<String> CompileResult::extractHlslDeclNames(const String& hlsl, const String& declType) {
    std::vector<String> retVal;

    String::Iterator declPos = hlsl.findFirst(declType+" ");
    while (declPos != hlsl.end()) {
        String::Iterator nameStart = declPos + declType.length() + 1;
        Parser::skip(nameStart, Unicode::isSpace);
        String::Iterator nameEnd = nameStart;
        Parser::skip(nameEnd, Parser::isIdentifierCharacter);

        retVal.push_back(hlsl.substr(nameStart, nameEnd));

        declPos = hlsl.findFirst(declType+" ", nameEnd);
    }

    return retVal;
}

const std::vector<String> CompileResult::extractCBufferNames(const String& hlsl) {
    return extractHlslDeclNames(hlsl, "cbuffer");
}

const std::vector<String> CompileResult::extractTextureInputs(const String& hlsl) {
    return extractHlslDeclNames(hlsl, "Texture2D");
}

CompileResult::CBuffer CompileResult::parseCBuffer(const String& hlsl, const String& cBufferName) {
    CompileResult::CBuffer parsedCBuffer;
    parsedCBuffer.name = cBufferName;

    String structDecl = "cbuffer " + cBufferName;
    String::Iterator before = hlsl.findFirst(structDecl) + structDecl.length();
    // Space before {
    Parser::skip(before, Unicode::isSpace);
    // {
    Parser::expectFixed(before, L'{');

    // Whitespace before first type
    Parser::skip(before, Unicode::isSpace);
    while (*before != L'}') {
        CompileResult::CBuffer::Member member;

        String::Iterator after = before;
        // Type
        Parser::skip(after, std::not_fn(Unicode::isSpace));
        member.type = hlsl.substr(before, after);
        // Whitespace after type
        Parser::skip(after, Unicode::isSpace);

        before = after;
        // Member name
        Parser::skip(after, Parser::isIdentifierCharacter);
        member.name = hlsl.substr(before, after);

        if (member.type.length() > 2 && member.type.substr(0, 2) != "//") {
            parsedCBuffer.members.push_back(member);
        }

        before = after;
        // Skip semicolon
        before++;

        // Whitespace before next type
        Parser::skip(before, Unicode::isSpace);
    }

    return parsedCBuffer;
}

void CompileResult::extractFunctionData(const String& hlsl, const String& functionName, CompileResult& compileResult) {
    String::Iterator iter = hlsl.begin();
    while (iter < hlsl.end()) {
        iter = hlsl.findFirst(functionName, iter) + functionName.length();
        Parser::skip(iter, Unicode::isSpace);

        if (*iter == '(') {
            //Correct function found!

            String::Iterator inputTypeStart = iter + 1;
            Parser::skip(inputTypeStart, Unicode::isSpace);
            String::Iterator inputTypeEnd = inputTypeStart;
            Parser::skip(inputTypeEnd, std::not_fn(Unicode::isSpace));
            String inputTypeName = hlsl.substr(inputTypeStart, inputTypeEnd);

            String::Iterator inputParamStart = inputTypeEnd;
            Parser::skip(inputParamStart, Unicode::isSpace);
            String::Iterator inputParamEnd = inputParamStart;
            Parser::skip(inputParamEnd, Parser::isIdentifierCharacter);
            String inputParamName = hlsl.substr(inputParamStart, inputParamEnd);

            String::ReverseIterator returnTypeEnd = (iter - functionName.length() - 1);
            Parser::skip(returnTypeEnd, Unicode::isSpace); returnTypeEnd--;
            String::ReverseIterator returnTypeStart = (returnTypeEnd + 1);
            Parser::skip(returnTypeStart, std::not_fn(Unicode::isSpace)); returnTypeStart--;
            String returnTypeName = hlsl.substr(returnTypeStart, returnTypeEnd);

            String::Iterator bodyStart = iter + 1;
            Parser::skip(bodyStart, std::not_fn(Parser::isOpeningBrace));
            String::Iterator bodyEnd = bodyStart;
            Parser::skipBlock(bodyEnd);

            compileResult.inputType = parseHlslStruct(hlsl, inputTypeName);
            compileResult.inputParameterName = inputParamName;
            compileResult.returnType = parseHlslStruct(hlsl, returnTypeName);
            compileResult.hlslFunctionBody = hlsl.substr(bodyStart, bodyEnd);

            return;
        }
    }
    throw PGE_CREATE_EX("Failed to locate function \"" + functionName + "\"");
}

std::vector<CompileResult::HlslStruct::Member>::const_iterator CompileResult::HlslStruct::findMember(const PGE::String& semanticName, int semanticIndex) const {
    for (auto it = members.begin(); it < members.end(); it++) {
        if ((it->dxSemanticName == semanticName) && (it->dxSemanticIndex == semanticIndex)) { return it; }
    }
    return members.end();
}

bool CompileResult::CBuffer::usedByFunction(const PGE::String& functionBody) {
    for (Member member : members) {
        if (functionBody.findFirst(member.name) != functionBody.end()) { return true; }
    }
    return false;
}

#include "Parser.h"
#include "CompileResult.h"

using namespace PGE;

CompileResult::HlslStruct CompileResult::parseHlslStruct(const String& hlsl, const String& structName) {
    // struct VS_INPUT {
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

std::vector<String> CompileResult::extractCBufferNames(const String& hlsl) {
    std::vector<String> retVal;

    String::Iterator cBufferPos = hlsl.findFirst("cbuffer ");
    while (cBufferPos != hlsl.end()) {
        String::Iterator nameStart = cBufferPos + 8;
        Parser::skip(nameStart, Unicode::isSpace);
        String::Iterator nameEnd = nameStart;
        Parser::skip(nameEnd, Parser::isIdentifierCharacter);

        retVal.push_back(hlsl.substr(nameStart, nameEnd));

        cBufferPos = hlsl.findFirst("cbuffer ", nameEnd);
    }

    return retVal;
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

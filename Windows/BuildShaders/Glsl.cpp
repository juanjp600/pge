#include <PGE/String/String.h>
#include <PGE/String/Unicode.h>

#include "Glsl.h"
#include "Parser.h"

using namespace PGE;

namespace Glsl {
    String hlslToGlslTypes(const String& hlsl) {
        return hlsl
            .replace("float2", "vec2")
            .replace("float3", "vec3")
            .replace("float4", "vec4")
            .replace("matrix", "mat4");
    }

    void writeHlslFuncs(TextWriter& writer, const String& funcBody) {
        //write definitions for intrinsic functions that the shader actually uses
        if (funcBody.findFirst("mul") != funcBody.end()) {
            writer.writeLine("vec4 mul(mat4 m, vec4 v) {");
            writer.writeLine("    return m * v;");
            writer.writeLine("}\n");
        }
    }

    String prefixIfRequired(const String& varName, const String& varKind, ShaderType shaderType) {
        if (((varKind == "out") && (shaderType == ShaderType::VERTEX)) ||
            ((varKind == "in") && (shaderType == ShaderType::FRAGMENT))) {
            return "vsToFs_" + varName;
        }
        return varName;
    }

    void writeStructDef(TextWriter& writer, const CompileResult::HlslStruct& hlslStruct) {
        writer.writeLine("struct " + hlslStruct.name + " {");
        for (CompileResult::HlslStruct::Member member : hlslStruct.members) {
            writer.writeLine("    " + hlslToGlslTypes(member.type) + " " + member.name + ";");
        }
        writer.writeLine("};\n");
    }

    void writeStructReturnAux(TextWriter& writer, const CompileResult::HlslStruct& hlslStruct, ShaderType shaderType) {
        writer.writeLine("void ret_aux(in " + hlslStruct.name + " retVal) {");
        for (CompileResult::HlslStruct::Member member : hlslStruct.members) {
            writer.writeLine("    " + prefixIfRequired(member.name, "out", shaderType) + " = retVal." + member.name + ";");
            if (member.dxSemanticName.equalsIgnoreCase("SV_POSITION")) {
                writer.writeLine("    gl_Position = retVal." + member.name + ";");
            }
        }
        writer.writeLine("}\n");

        writer.writeLine(hlslStruct.name + " def_output_value() {");
        writer.writeLine("    return " + hlslStruct.name + "(");
        for (int i = 0; i < hlslStruct.members.size(); i++) {
            CompileResult::HlslStruct::Member member = hlslStruct.members[i];
            String ln = "        ";
            //Worth noting: in spite of the fact that the
            //GLSL spec does allow initializing from a single
            //value, there are bad implementations out there
            //that don't allow it >:(
            if (member.type == "float2") {
                ln += "vec2(0.0,0.0)";
            }
            else if (member.type == "float3") {
                ln += "vec3(0.0,0.0,0.0)";
            }
            else if (member.type == "float4") {
                ln += "vec4(0.0,0.0,0.0,0.0)";
            }
            else if (member.type == "matrix") {
                ln += "mat4(1.0)";
            }
            else {
                ln += "0";
            }
            ln += ((i < (hlslStruct.members.size() - 1)) ? "," : ");");
            writer.writeLine(ln);
        }
        writer.writeLine("}\n");
    }

    void writeStructAsVars(TextWriter& writer, const CompileResult::HlslStruct& hlslStruct, const String& varKind, ShaderType shaderType) {
        for (CompileResult::HlslStruct::Member member : hlslStruct.members) {
            writer.writeLine(varKind + " " + hlslToGlslTypes(member.type) + " " + prefixIfRequired(member.name, varKind, shaderType) + ";");
        }
        writer.writeLine("");
    }

    void writeMain(TextWriter& writer, const CompileResult& compileResult, ShaderType shaderType) {
        String body = compileResult.hlslFunctionBody;
        body = hlslToGlslTypes(body)
            .replace(compileResult.inputParameterName + ".", prefixIfRequired("", "in", shaderType))
            .replace("output", "out_put") //reserved keyword in glsl
            .replace("input", "in_put") //reserved keyword in glsl
            .replace("(" + compileResult.returnType.name + ")0", "def_output_value()");

        String::Iterator returnStatementPos = body.findFirst("return");
        while (returnStatementPos != body.end()) {
            String pre = body.substr(body.begin(), returnStatementPos);

            String::Iterator valueStart = returnStatementPos + 6;
            Parser::skip(valueStart, Unicode::isSpace);
            String::Iterator valueEnd = valueStart;
            Parser::skipStatement(valueEnd);

            String value = body.substr(valueStart, valueEnd);

            String post = body.substr(valueEnd, body.end());

            body = pre + "ret_aux(" + value + ")" + post;

            returnStatementPos = body.findFirst("return");
        }

        writer.writeLine("void main() " + body);
    }

    void convert(const FilePath& path, const CompileResult& compileResult, ShaderType shaderType) {
        TextWriter writer(path);
        writer.writeLine("//This file was autogenerated.\n#version 330 core\n");

        //TODO: uniforms

        writeStructAsVars(writer, compileResult.inputType, "in", shaderType);
        writeStructAsVars(writer, compileResult.returnType, "out", shaderType);

        writeHlslFuncs(writer, compileResult.hlslFunctionBody);

        writeStructDef(writer, compileResult.returnType);
        writeStructReturnAux(writer, compileResult.returnType, shaderType);

        writeMain(writer, compileResult, shaderType);
    }
}
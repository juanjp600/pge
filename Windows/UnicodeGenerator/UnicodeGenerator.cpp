#include <fstream>
#include <iostream>
#include <unordered_set>

#include <PGE/String/String.h>
#include <PGE/String/Key.h>
#include <PGE/Exception/Exception.h>
#include <PGE/File/TextWriter.h>
#include <PGE/File/TextReader.h>

using namespace PGE;

#define PGE_UNICODE_ASSERT_FILE(stream) PGE_ASSERT(stream.good(), "Stream is not good!")

static String paramsToMapType(const String& key, const String& value) {
    return "std::unordered_map<" + key + ", " + value + ">";
}

static const String INDENT = "    ";
static const String TAIL = "};";
static const String MAP_TYPE = paramsToMapType("char16", "char16");
static const String MULTI_FOLD_MAP_TYPE = paramsToMapType("char16", "std::vector<char16>");

struct Mapping {
    String from;
    String to;
};

struct MultiMapping {
    String from;
    std::vector<String> to;
};

static String rawToChar(const String& str) {
    return "u'\\u" + str + "'";
}

static String entriesToPair(const String& a, const String& b) {
    return "{ " + rawToChar(a) + ", " + rawToChar(b) + " },";
}

static String toMapHead(const String& mapType, const String& identifier) {
    return "const " + mapType + " Unicode::" + identifier + " = " + mapType + " {";
}

static void writeMappings(TextWriter& writer, const std::vector<Mapping>& mappings) {
    for (const Mapping& mapping : mappings) {
        writer.writeLine(INDENT + entriesToPair(mapping.from, mapping.to));
    }
}

static void writeMappings(TextWriter& writer, const std::vector<MultiMapping>& mappings) {
    for (const MultiMapping& mapping : mappings) {
        writer.write(INDENT + "{ " + rawToChar(mapping.from) + ", { ");
        for (const String& str : mapping.to) {
            writer.write(rawToChar(str) + ", ");
        }
        writer.writeLine("} },");
    }
}

int main() {
    TextWriter out(FilePath::fromStr("../../Src/String/Unicode.cpp"));

    out.writeLine(
R"(
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                   //
//       ###    ##     ## ########  #######   ######   ######## ##    ## ######## ########     ###    ######## ######## ########     //
//      ## ##   ##     ##    ##    ##     ## ##    ##  ##       ###   ## ##       ##     ##   ## ##      ##    ##       ##     ##    //
//     ##   ##  ##     ##    ##    ##     ## ##        ##       ####  ## ##       ##     ##  ##   ##     ##    ##       ##     ##    //
//    ##     ## ##     ##    ##    ##     ## ##   #### ######   ## ## ## ######   ########  ##     ##    ##    ######   ##     ##    //
//    ######### ##     ##    ##    ##     ## ##    ##  ##       ##  #### ##       ##   ##   #########    ##    ##       ##     ##    //
//    ##     ## ##     ##    ##    ##     ## ##    ##  ##       ##   ### ##       ##    ##  ##     ##    ##    ##       ##     ##    //
//    ##     ##  #######     ##     #######   ######   ######## ##    ## ######## ##     ## ##     ##    ##    ######## ########     //
//                                                                                                                                   //
//    ######## #### ##       ########  //  ########   #######     ##    ##  #######  ########    ######## ########  #### ########    //
//    ##        ##  ##       ##        //  ##     ## ##     ##    ###   ## ##     ##    ##       ##       ##     ##  ##     ##       //
//    ##        ##  ##       ##        //  ##     ## ##     ##    ####  ## ##     ##    ##       ##       ##     ##  ##     ##       //
//    ######    ##  ##       ######    //  ##     ## ##     ##    ## ## ## ##     ##    ##       ######   ##     ##  ##     ##       //
//    ##        ##  ##       ##        //  ##     ## ##     ##    ##  #### ##     ##    ##       ##       ##     ##  ##     ##       //
//    ##        ##  ##       ##        //  ##     ## ##     ##    ##   ### ##     ##    ##       ##       ##     ##  ##     ##       //
//    ##       #### ######## ########  //  ########   #######     ##    ##  #######     ##       ######## ########  ####    ##       //
//                                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
)" + 1
    );
    out.writeLine("#include \"Unicode.h\"");
    out.writeLine();
    out.writeLine("using namespace PGE;");
    out.writeLine();

    std::vector<Mapping> fullFolding;

    TextReader in(FilePath::fromStr("CaseFolding.txt"));

    out.writeLine(toMapHead(MAP_TYPE, "FOLDING"));
    String line;
    while (!in.endOfFile()) {
        line = String();
        in.readLine(line);
        if (line.byteLength() == 0 || *line.charAt(0) == L'#') { continue; }
        std::vector<String> params = line.split(";", false);
        params[0] = params[0].trim();
        // We reached UTF-32 range!
        if (params[0].byteLength() == 5) { break; }
        char16 type = *params[1].trim().charAt(0);
        if (type == 'C') {
            out.writeLine(INDENT + entriesToPair(params[0], params[2].trim()));
        } else if (type == 'F') {
            out.writeLine(INDENT + entriesToPair(params[0], "FFFF"));
            fullFolding.push_back({ params[0], params[2].trim() });
        }
    }
    out.writeLine(TAIL);

    out.writeLine();
    out.writeLine(toMapHead(MULTI_FOLD_MAP_TYPE, "MULTI_FOLDING"));
    for (Mapping& mapping : fullFolding) {
        out.write(INDENT + "{ " + rawToChar(mapping.from) + ", { ");
        std::vector<String> splitChars = mapping.to.split(" ", true);
        for (String& ch : splitChars) {
            out.write(rawToChar(ch) + ", ");
        }
        out.writeLine("} },");
    }
    out.writeLine(TAIL);

    out.writeLine();

    std::vector<Mapping> up;
    std::vector<Mapping> down;
    std::vector<MultiMapping> multiUp;
    std::vector<MultiMapping> multiDown;
    std::unordered_set<String::Key> setUp;
    std::unordered_set<String::Key> setDown;

    // MANUAL OVERRIDE
    up.push_back({ "00DF", "1E9E" });
    setUp.insert(String("00DF"));
    //

    in = TextReader(FilePath::fromStr("SpecialCasing.txt"));
    while (!in.endOfFile()) {
        line = String();
        in.readLine(line);
        if (line.byteLength() == 0 || *line.charAt(0) == L'#') { continue; }
        std::vector<String> params = line.split(";", false);
        // We don't want any conditional mappings.
        if (*params[4].trim().charAt(0) != L'#') { continue; }
        if (params[3].byteLength() != 0) {
            std::vector<String> split = params[3].split(" ", true);
            // It must actually multi-fold.
            if (split.size() != 1 && setUp.find(params[0]) == setUp.end()) {
                multiUp.push_back({ params[0], split });
                up.push_back({ params[0], "FFFF" });
                setUp.insert(params[0]);
            }
        }
        if (params[1].byteLength() != 0) {
            std::vector<String> split = params[1].split(" ", true);
            // It must actually multi-fold.
            if (split.size() != 1 && setDown.find(params[0]) == setDown.end()) {
                multiDown.push_back({ params[0], split });
                down.push_back({ params[0], "FFFF" });
                setDown.insert(params[0]);
            }
        }
    }

    in = TextReader(FilePath::fromStr("UnicodeData.txt"));
    while (!in.endOfFile()) {
        line = String();
        in.readLine(line);
        if (line.byteLength() == 0 || *line.charAt(0) == L'#') { continue; }
        std::vector<String> params = line.split(";", false);
        // We reached UTF-32 range!
        if (params[0].byteLength() == 5) { break; }
        if (params[12].byteLength() != 0 && setUp.find(params[0]) == setUp.end()) {
            up.push_back({ params[0], params[12] });
        }
        if (params[13].byteLength() != 0 && setDown.find(params[0]) == setDown.end()) {
            down.push_back({ params[0], params[13] });
        }
    }

    out.writeLine(toMapHead(MAP_TYPE, "UP"));
    writeMappings(out, up);
    out.writeLine(TAIL);

    out.writeLine();

    out.writeLine(toMapHead(MAP_TYPE, "DOWN"));
    writeMappings(out, down);
    out.writeLine(TAIL);

    out.writeLine();

    out.writeLine(toMapHead(MULTI_FOLD_MAP_TYPE, "MULTI_UP"));
    writeMappings(out, multiUp);
    out.writeLine(TAIL);

    out.writeLine();

    out.writeLine(toMapHead(MULTI_FOLD_MAP_TYPE, "MULTI_DOWN"));
    writeMappings(out, multiDown);
    out.writeLine(TAIL);

    std::cout << "Generated Unicode code!" << std::endl;
    return 0;
}

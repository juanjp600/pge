#include <iostream>
#include <unordered_set>
#include <set>
#include <map>

#include <PGE/String/Key.h>
#include <PGE/File/TextWriter.h>
#include <PGE/File/TextReader.h>

using namespace PGE;

static const String INDENT = "    ";

class SwitchWriter {
    protected:
        using Cases = std::set<String::OrderedKey>;
        using StatementMap = std::map<String::OrderedKey, Cases>;

        SwitchWriter(const String& name, const String& param, const String& preAction, const String& postAction)
            : name(name), param(param), preAction(preAction), postAction(postAction) { }

        void write(TextWriter& out, const StatementMap& done) const {
            out.writeLine("void Unicode::" + name + "(" + param + ", char16 ch) {");
            out.writeLine(INDENT + "switch (ch) {");
            for (const auto& it : done) {
                std::vector<String> chars = it.first.str.split(" ", true);
                bool first = true;
                for (const auto& sit : it.second) {
                    if (first) {
                        first = false;
                    } else {
                        out.writeLine();
                    }
                    out.write(INDENT.multiply(2) + "case " + rawToChar(sit.str) + ":");
                }
                out.writeLine(" {");
                for (const String& toChr : chars) {
                    writeAction(out, rawToChar(toChr));
                }
                writeBreak(out);
            }
            out.writeLine(INDENT.multiply(2) + "default: {");
            writeAction(out, "ch");
            writeBreak(out);
            out.writeLine(INDENT + "}");
            out.writeLine("}");
        }

    private:
        const String name;
        const String param;
        const String preAction;
        const String postAction;

        void writeAction(TextWriter& out, const String& item) const {
            out.writeLine(INDENT.multiply(3) + preAction + item + postAction);
        }

        static void writeBreak(TextWriter& out) {
            out.writeLine(INDENT.multiply(2) + "} break;");
        }

        static String rawToChar(const String& str) {
            return "u'\\u" + str + "'";
        }
};

// Both SwitchWriter implementations constitute degenerate builders.
class Folder : private SwitchWriter {
    public:
        Folder(const String& name)
            : SwitchWriter(name, "std::queue<char16>& queue", "queue.push(", ");") { }

        void feed(const String& from, const String& to, bool dominant) {
            // It dominantes!
            if (dominant) {
                pre[from] = to;
            } else {
                // We haven't mapped this yet!
                if (pre.find(from) == pre.end()) {
                    pre.emplace(from, to);
                }
            }
        }

        void write(TextWriter& out) const {
            StatementMap done;
            for (const auto& it : pre) {
                done[it.second.str].emplace(it.first.str);
            }

            SwitchWriter::write(out, done);
        }

    private:
        std::unordered_map<String::RedundantKey, String::RedundantKey> pre;
};

class Caser : private SwitchWriter {
    public:
        Caser(const String& name)
            : SwitchWriter(name, "String& str", "str += ", ";") { }

        void feed(const String& from, const String& to) {
            // First come, first serve.
            if (hasHad.find(from) == hasHad.end()) {
                done[to].emplace(from);
                hasHad.emplace(from);
            }
        }

        void write(TextWriter& out) const {
            SwitchWriter::write(out, done);
        }

    private:
        StatementMap done;
        std::unordered_set<String::Key> hasHad;
};

namespace UnicodeParser {
    using Consumer = std::function<void(const std::vector<String>&)>;

    void parse(const FilePath& path, const Consumer& func) {
        TextReader in(path);
        static String line; // Fuck it, static line cache. (Remove this if used elsewhere, thx.)
        while (!in.endOfFile()) {
            line = String();
            in.readLine(line);
            if (line.byteLength() == 0 || *line.charAt(0) == L'#') { continue; }
            std::vector<String> params = line.split(";", false);
            params[0] = params[0].trim();
            // We reached UTF-32 range!
            if (params[0].byteLength() == 5) { break; }
            func(params);
        }
    }
};

int main() {
    TextWriter out(FilePath::fromStr("../../Src/String/Unicode.cpp"));

    // Incase it wasn't obvious, this doesn't apply to this file.
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
)" + 1 // + 1 to skip leading newline.
    );
    out.writeLine("#include \"Unicode.h\"");
    out.writeLine();
    out.writeLine("using namespace PGE;");
    out.writeLine();

    Folder foldedSwitch("fold");
    UnicodeParser::parse(FilePath::fromStr("CaseFolding.txt"), [&](const std::vector<String>& params) {
        char16 type = *params[1].trim().charAt(0);
        // Full/Multi-char folding dominates!
        foldedSwitch.feed(params[0], params[2], type == L'F');
    });
    foldedSwitch.write(out);

    out.writeLine();

    Caser upSwitch("up");
    Caser downSwitch("down");

    // MANUAL OVERRIDE
    upSwitch.feed("00DF", "1E9E");
    //

    UnicodeParser::parse(FilePath::fromStr("SpecialCasing.txt"), [&](const std::vector<String>& params) {
        // We don't want any conditional mappings.
        if (*params[4].trim().charAt(0) != L'#') { return; }
        if (params[3].byteLength() != 0) {
            upSwitch.feed(params[0], params[3]);
        }
        if (params[1].byteLength() != 0) {
            downSwitch.feed(params[0], params[1]);
        }
    });

    UnicodeParser::parse(FilePath::fromStr("UnicodeData.txt"), [&](const std::vector<String>& params) {
        if (params[12].byteLength() != 0) {
            upSwitch.feed(params[0], params[12]);
        }
        if (params[13].byteLength() != 0) {
            downSwitch.feed(params[0], params[13]);
        }
    });

    upSwitch.write(out);

    out.writeLine();

    downSwitch.write(out);

    std::cout << "Generated Unicode code!" << std::endl;
    return 0;
}

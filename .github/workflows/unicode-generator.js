const fs = require("fs");
const cp = require("child_process");

if (process.argv.length !== 3) {
    console.log("Wrong command line arguments");
    return;
}
const fileName = process.argv[2];

const header = fs.readFileSync(`${__dirname}/unicode-header.txt`);

const file = {
    contents: header,
    write: function(cnt) {
        this.contents += cnt;
    },
    writeLine: function(cnt = "") { this.write(cnt + "\n") },
    writeSwitch: function(map, name, param, action, retType = "void", defaultAction = () => action("ch")) {
        const strCmp = (a, b) => a < b ? -1 : (b > a ? 1 : 0);
        const codePointToCharLiteral = (codepoint) => `u'\\u${codepoint}'`;

        let arr = [];
        for (const key in map) {
            arr.push({ key: key, value: map[key] });
        }
        arr.sort((a, b) => strCmp(a.value[0], b.value[0]));

        const INDENT = "    ";
        const optionalParam = param.length !== 0 ? (param + ", ") : "";
        this.writeLine(`${retType} Unicode::${name}(${optionalParam}char16 ch) {`);
        this.writeLine(`${INDENT}switch (ch) {`);
        for (const mapping of arr) {
            let first = true;
            for (const mappedChar of mapping.value.sort(strCmp)) {
                if (first) { first = false; }
                else { this.writeLine(" [[fallthrough]];"); }
                this.write(`${INDENT.repeat(2)}case ${codePointToCharLiteral(mappedChar)}:`);
            }
            this.writeLine(" {");
            for (const mapped of mapping.key.split(" ")) {
                this.writeLine(INDENT.repeat(3) + action(codePointToCharLiteral(mapped)));
            }
            this.writeLine(`${INDENT.repeat(2)}} break;`);
        }
        this.writeLine(`${INDENT.repeat(2)}default: {`);
        this.writeLine(INDENT.repeat(3) + defaultAction());
        this.writeLine(`${INDENT.repeat(2)}} break;`);
    
        this.writeLine(INDENT + "}");
        this.writeLine("}");
    }
}

class Caser {
    #cases = {};
    #hasHad = new Set();
    feed(from, to) {
        from = from.trim();
        to = to.trim();
        if (to.length === 0 || from === to) { return; }
        // First come, first served
        if (!this.#hasHad.has(from)) {
            this.#hasHad.add(from);
            this.#cases[to] = this.#cases[to] || [];
            this.#cases[to].push(from);
        }
    }
    write(file, name) {
        file.writeSwitch(this.#cases, name, "String& str", (ch) => `str += ${ch};`);
    }
}

let map = {};
parseUnicode("https://www.unicode.org/Public/UCD/latest/ucd/CaseFolding.txt", (params) => {
    // It's full/multi-char folding or not yet folded!
    params[0] = params[0].trim();
    params[2] = params[2].trim();
    if (params[0] !== params[2] && (params[1].trim() === "F" || !(params[0] in map))) {
        map[params[0]] = params[2];
    }
});
let flipMap = {};
for (const key in map) {
    flipMap[map[key]] = flipMap[map[key]] || [];
    flipMap[map[key]].push(key);
}
file.writeSwitch(flipMap, "fold", "CircularArray<char16>& queue", (ch) => `queue.pushBack(${ch});`);
file.writeLine();

const up = new Caser();
const down = new Caser();
// MANUAL OVERRIDE
up.feed("00DF", "1E9E");
//
parseUnicode("https://unicode.org/Public/UNIDATA/SpecialCasing.txt", (params) => {
    // We don't want any conditional mappings
    if (params[4].trim()[0] !== "#") { return; }
    up.feed(params[0], params[3]);
    down.feed(params[0], params[1]);
});

let spaceIdent = [];
parseUnicode("https://unicode.org/Public/UNIDATA/UnicodeData.txt", (params) => {
    up.feed(params[0], params[12]);
    down.feed(params[0], params[13]);
    if (params[2][0] === "Z" || params[2] === "Cc") {
        spaceIdent.push(params[0]);
    }
});

up.write(file, "up");
file.writeLine();
down.write(file, "down");
file.writeLine();
file.writeSwitch({spaceIdent}, "isSpace", "", () => "return true;", "bool", () => "return false;");
fs.writeFileSync(fileName, file.contents);

console.log("Generated Unicode code!")

function parseUnicode(url, func) {
    const unicode = cp.execSync(`curl ${url}`, { maxBuffer: 10_000_000 }).toString();
    for (const line of unicode.split("\n")) {
        if (line.length === 0 || line[0] === "#") { continue; }
        const params = line.split(";");
        // We reached UTF-32 range!
        if (params[0].length === 5) { break; }
        func(params);
    }
}

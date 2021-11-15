const fs = require("fs");

if (process.argv.length !== 3) {
    console.error("No file provided!");
    return;
}

const fileName = process.argv[2];

fs.readFile(fileName, (err, data) => {
    if (err) {
        console.error(err);
        return;
    }

    let categories = [];
    class Category {
        constructor(name) {
            this.name = name;
            this.elements = [];
        }
    }

    const file = data.toString();
    let index = 0;
    while ((index = file.indexOf("\n#", index)) !== -1) {
        index += 2;

        let newCategory;

        if (file.charAt(index) !== '#') {
            index++;
            newCategory = true;
        } else {
            index++;
            if(file.charAt(index) === '#') { // Ignore headings > level 2.
                index = file.indexOf("\n", index);
                continue;
            }

            index++;
            newCategory = false;
        }

        const lineEnd = file.indexOf("\n", index);
        const name = file.substr(index, lineEnd - index);
        index = lineEnd;

        if (newCategory) {
            categories.push(new Category(name));
        } else {
            categories[categories.length - 1].elements.push(name);
        }
    }

    let newFile = "";
    for (const category of categories) {
        function makeEntry(str) { return `[${str}](#${str.toLowerCase().replaceAll(" ", "-").replace(/[^a-zA-Z0-9-_]/g, "")})\n` }
        newFile += "- " + makeEntry(category.name);
        for (const topic of category.elements) {
            newFile += "    * " + makeEntry(topic);
        }
    }

    newFile += '\n';

    newFile += file;

    const lastDot = fileName.lastIndexOf(".");
    fs.writeFile(fileName.substr(0, lastDot) + "_TOC" + fileName.substr(lastDot), newFile, err => {
        if (err) {
          console.error(err);
        }
    });
});

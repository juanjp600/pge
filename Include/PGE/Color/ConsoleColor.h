#ifndef PGE_CONSOLECOLOR_H_INCLUDED
#define PGE_CONSOLECOLOR_H_INCLUDED

#include "Color.h"

namespace PGE {

// Reference: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#colors--graphics-mode
namespace Console {
    struct ForegroundColor { Color color; };
    struct BackgroundColor { Color color; };

    struct Bold { bool bold; };
    struct Dim { bool dim; };
    struct Italic { bool italic; };
    struct Underline { bool underline; };
    struct Blinking { bool blinking; };
    struct Reverse { bool reverse; };
    struct Hidden { bool hidden; };
    struct Strikethrough { bool strikethrough; };

    struct ResetAll { };
}

}

std::ostream& operator<<(std::ostream& out, const PGE::Console::ForegroundColor& color);
std::ostream& operator<<(std::ostream& out, const PGE::Console::BackgroundColor& color);

std::ostream& operator<<(std::ostream& out, const PGE::Console::Bold& bold);
std::ostream& operator<<(std::ostream& out, const PGE::Console::Dim& dim);
std::ostream& operator<<(std::ostream& out, const PGE::Console::Italic& italic);
std::ostream& operator<<(std::ostream& out, const PGE::Console::Underline& underline);
std::ostream& operator<<(std::ostream& out, const PGE::Console::Blinking& blinking);
std::ostream& operator<<(std::ostream& out, const PGE::Console::Reverse& reverse);
std::ostream& operator<<(std::ostream& out, const PGE::Console::Hidden& hidden);
std::ostream& operator<<(std::ostream& out, const PGE::Console::Strikethrough& strikethrough);

std::ostream& operator<<(std::ostream& out, const PGE::Console::ResetAll&);

#endif // PGE_CONSOLECOLOR_H_INCLUDED

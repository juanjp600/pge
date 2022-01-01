#include <PGE/Color/ConsoleColor.h>

#include <iostream>

using namespace PGE;

template <int code>
static std::ostream& writeColor(std::ostream& out, const Color& color) {
    return out << "\x1b[" << code << ";2;" << (int)color.getRed() << ";" << (int)color.getGreen() << ";" << (int)color.getBlue() << "m";
}

std::ostream& operator<<(std::ostream& out, const Console::ForegroundColor& color) {
    return writeColor<38>(out, color.color);
}

std::ostream& operator<<(std::ostream& out, const Console::BackgroundColor& color) {
    return writeColor<48>(out, color.color);
}

template <int onTrue, int onFalse = onTrue + 20>
static std::ostream& writeFlag(std::ostream& out, bool flag) {
    return out << "\x1b[" << (flag ? onTrue : onFalse) << "m";
}

std::ostream& operator<<(std::ostream& out, const Console::Bold& bold) {
    return writeFlag<1, 22>(out, bold.bold);
}

std::ostream& operator<<(std::ostream& out, const Console::Dim& dim) {
    return writeFlag<2>(out, dim.dim);
}

std::ostream& operator<<(std::ostream& out, const Console::Italic& italic) {
    return writeFlag<3>(out, italic.italic);
}

std::ostream& operator<<(std::ostream& out, const Console::Underline& underline) {
    return writeFlag<4>(out, underline.underline);
}

std::ostream& operator<<(std::ostream& out, const Console::Blinking& blinking) {
    return writeFlag<5>(out, blinking.blinking);
}

std::ostream& operator<<(std::ostream& out, const Console::Reverse& reverse) {
    return writeFlag<7>(out, reverse.reverse);
}

std::ostream& operator<<(std::ostream& out, const Console::Hidden& hidden) {
    return writeFlag<8>(out, hidden.hidden);
}

std::ostream& operator<<(std::ostream& out, const Console::Strikethrough& strikethrough) {
    return writeFlag<9>(out, strikethrough.strikethrough);
}

std::ostream& operator<<(std::ostream& out, const Console::ResetAll&) {
    return out << "\x1b[0m";
}

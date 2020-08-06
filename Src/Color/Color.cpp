#include <Color/Color.h>

using namespace PGE;

Color::Color() {
    red = 1.f; green = 1.f; blue = 1.f; alpha = 1.f;
}

Color::Color(int r, int g, int b, int a) {
    red = ((float)r)/255.f; green = ((float)g)/255.f; blue = ((float)b)/255.f; alpha = ((float)a)/255.f;
}

Color::Color(float r, float g, float b, float a) {
    red = r; green = g; blue = b; alpha = a;
}

int Color::getRedInt() const {
    return (int)(red*255.f);
}

int Color::getGreenInt() const {
    return (int)(green*255.f);
}

int Color::getBlueInt() const {
    return (int)(blue*255.f);
}

int Color::getAlphaInt() const {
    return (int)(alpha*255.f);
}

void Color::setRedInt(int r) {
    red = ((float)r)/255.f;
}

void Color::setGreenInt(int g) {
    green = ((float)g)/255.f;
}

void Color::setBlueInt(int b) {
    blue = ((float)b)/255.f;
}

void Color::setAlphaInt(int a) {
    alpha = ((float)a)/255.f;
}

const Color Color::Red = Color(1.f, 0.f, 0.f);
const Color Color::Green = Color(0.f, 1.f, 0.f);
const Color Color::Blue = Color(0.f, 0.f, 1.f);
const Color Color::Orange = Color(1.f, 0.5f, 0.f);
const Color Color::Yellow = Color(1.f, 1.f, 0.f);
const Color Color::Cyan = Color(0.f, 1.f, 1.f);
const Color Color::Magenta = Color(1.f, 0.f, 1.f);
const Color Color::White = Color(1.f, 1.f, 1.f);
const Color Color::Gray = Color(0.5f, 0.5f, 0.5f);
const Color Color::Black = Color(0.f, 0.f, 0.f);

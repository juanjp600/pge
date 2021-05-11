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

bool Color::operator==(const Color& other) const {
    return red == other.red && blue == other.blue && green == other.green && alpha == other.alpha;
}

bool Color::operator!=(const Color& other) const {
    return red != other.red || blue != other.blue || green != other.green || alpha != other.alpha;
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

const Color Color::RED = Color(1.f, 0.f, 0.f);
const Color Color::GREEN = Color(0.f, 1.f, 0.f);
const Color Color::BLUE = Color(0.f, 0.f, 1.f);
const Color Color::ORANGE = Color(1.f, 0.5f, 0.f);
const Color Color::YELLOW = Color(1.f, 1.f, 0.f);
const Color Color::CYAN = Color(0.f, 1.f, 1.f);
const Color Color::MAGENTA = Color(1.f, 0.f, 1.f);
const Color Color::WHITE = Color(1.f, 1.f, 1.f);
const Color Color::GRAY = Color(0.5f, 0.5f, 0.5f);
const Color Color::BLACK = Color(0.f, 0.f, 0.f);

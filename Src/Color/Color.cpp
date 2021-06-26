#include <PGE/Color/Color.h>

#include <PGE/Exception/Exception.h>

using namespace PGE;

#define PGE_ASSERT_COLOR(color, name) PGE_ASSERT((color).name >= 0.f && (color).name <= 1.f, "Value is outside of valid range (" #name ": " + String::fromFloat((color).name) + ")")

static void validateColor(const Color& color) {
    PGE_ASSERT_COLOR(color, red);
    PGE_ASSERT_COLOR(color, green);
    PGE_ASSERT_COLOR(color, blue);
    PGE_ASSERT_COLOR(color, alpha);
}

Color::Color() {
    red = 1.f; green = 1.f; blue = 1.f; alpha = 1.f;
}

Color::Color(byte r, byte g, byte b, byte a) {
    red = r/255.f; green = g/255.f; blue = b/255.f; alpha = a/255.f;
    validateColor(*this);
}

Color::Color(float r, float g, float b, float a) {
    red = r; green = g; blue = b; alpha = a;
    validateColor(*this);
}

Color Color::fromHSV(float h, float s, float v, float alpha) {
    PGE_ASSERT(h >= 0 && h <= 360.f, "Hue is outside of valid range (hue: " + String::fromFloat(h) + ")");
    PGE_ASSERT(s >= 0 && s <= 1.f, "Saturation is outside of valid range (saturation: " + String::fromFloat(s) + ")");
    PGE_ASSERT(v >= 0 && v <= 1.f, "Value is outside of valid range (value: " + String::fromFloat(v) + ")");

    float hh = h / 60.f;
    int i = (int)hh;
    float dr = hh - i; // Decimal remainder.

    float x = v * (1.f - s);
    float y = v * (1.f - (s * dr));
    float z = v * (1.f - (s * (1.f - dr)));

    switch (i) {
        default:
        case 0: {
            return Color(v, z, x);
        }
        case 1: {
            return Color(y, v, x);
        }
        case 2: {
            return Color(x, v, z);
        }
        case 3: {
            return Color(x, y, v);
        }
        case 4: {
            return Color(z, x, v);
        }
        case 5: {
            return Color(v, x, y);
        }
    }
}

bool Color::operator==(const Color& other) const {
    return red == other.red && blue == other.blue && green == other.green && alpha == other.alpha;
}

bool Color::operator!=(const Color& other) const {
    return red != other.red || blue != other.blue || green != other.green || alpha != other.alpha;
}

byte Color::getRed() const {
    return (byte)(red*255.f);
}

byte Color::getGreen() const {
    return (byte)(green*255.f);
}

byte Color::getBlue() const {
    return (byte)(blue*255.f);
}

byte Color::getAlpha() const {
    return (byte)(alpha*255.f);
}

void Color::setRed(byte r) {
    red = ((float)r)/255.f;
    PGE_ASSERT_COLOR(*this, red);
}

void Color::setGreen(byte g) {
    green = ((float)g)/255.f;
    PGE_ASSERT_COLOR(*this, green);
}

void Color::setBlue(byte b) {
    blue = ((float)b)/255.f;
    PGE_ASSERT_COLOR(*this, blue);
}

void Color::setAlpha(byte a) {
    alpha = ((float)a)/255.f;
    PGE_ASSERT_COLOR(*this, alpha);
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

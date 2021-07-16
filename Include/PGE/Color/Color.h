#ifndef PGE_COLOR_H_INCLUDED
#define PGE_COLOR_H_INCLUDED

#include <PGE/Math/Math.h>
#include <PGE/Types/Types.h>
#include <PGE/Exception/Exception.h>

namespace PGE {

/// An RGBA color represented via 4 floating point numbers.
/// Attributes are public.
/// @see #PGE::Colors
class Color {
    public:
        float red; float green; float blue; float alpha;

        /// Default constructs fully opaque white (#PGE::Colors::WHITE).
        constexpr Color() noexcept : red(1.f), green(1.f), blue(1.f), alpha(1.f) { }
        /// Converts bytes in range [0, 255] to floats in range [0, 1].
        constexpr Color(byte r, byte g, byte b, byte a = 255) noexcept : red(r / 255.f), green(g / 255.f), blue(b / 255.f), alpha(a / 255.f) { }
        /// Values should be in range [0, 1].
        constexpr Color(float r, float g, float b, float a = 1.f) noexcept : red(r), green(g), blue(b), alpha(a) { }

        /// Converts HSV(A) to RGBA.
        /// @param[in] h The hue, must be in range [0, 360].
        /// @param[in] s The saturation, must be in range [0, 1].
        /// @param[in] v The value, must be in range [0, 1].
        /// @param[in] a The alpha, should be in range [0, 1].
        /// @exception #PGE::Exception If hue, saturation or value are outside their expected range.
        /// @see https://en.wikipedia.org/wiki/HSL_and_HSV
        static constexpr const Color fromHSV(float h, float s, float v, float a = 1.f) {
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
                    return Color(v, z, x, a);
                }
                case 1: {
                    return Color(y, v, x, a);
                }
                case 2: {
                    return Color(x, v, z, a);
                }
                case 3: {
                    return Color(x, y, v, a);
                }
                case 4: {
                    return Color(z, x, v, a);
                }
                case 5: {
                    return Color(v, x, y, a);
                }
            }
        }

        /// Uses regular float equality.
        constexpr bool operator==(const Color& other) const noexcept {
            return red == other.red && blue == other.blue && green == other.green && alpha == other.alpha;
        }

        /// Uses regular float equality.
        constexpr bool operator!=(const Color& other) const noexcept {
            return red != other.red || blue != other.blue || green != other.green || alpha != other.alpha;
        }

        /// @see #PGE::Math::equalFloats()
        constexpr bool equals(const Color& other, float epsilon = Math::EPSILON_DEFAULT) const noexcept {
            return Math::equalFloats(red, other.red, epsilon)
                && Math::equalFloats(green, other.green, epsilon)
                && Math::equalFloats(blue, other.blue, epsilon)
                && Math::equalFloats(alpha, other.alpha, epsilon);
        }

        constexpr byte getRed() const noexcept { return (byte)(red * 255.f); }
        constexpr byte getGreen() const noexcept { return (byte)(green * 255.f); }
        constexpr byte getBlue() const noexcept { return (byte)(blue * 255.f); }
        constexpr byte getAlpha() const noexcept { return (byte)(alpha * 255.f); }
        constexpr void setRed(byte r) noexcept { red = ((float)r) / 255.f; }
        constexpr void setGreen(byte g) noexcept { green = ((float)g) / 255.f; }
        constexpr void setBlue(byte b) noexcept { blue = ((float)b) / 255.f; }
        constexpr void setAlpha(byte a) noexcept { alpha = ((float)a) / 255.f; }

        void* operator new(size_t) = delete;
        void* operator new[](size_t) = delete;
        void operator delete(void*) = delete;
        void operator delete[](void*) = delete;
};

/// A few common colors.
/// @see #PGE::Color
namespace Colors {
    constexpr Color RED = Color(1.f, 0.f, 0.f); ///< \#FF0000
    constexpr Color GREEN = Color(0.f, 1.f, 0.f); ///< \#00FF00
    constexpr Color BLUE = Color(0.f, 0.f, 1.f); ///< \#0000FF
    constexpr Color ORANGE = Color(1.f, 0.5f, 0.f); ///< \#FF7F00
    constexpr Color YELLOW = Color(1.f, 1.f, 0.f); ///< \#FFFF00
    constexpr Color CYAN = Color(0.f, 1.f, 1.f); ///< \#00FFFF
    constexpr Color MAGENTA = Color(1.f, 0.f, 1.f); ///< \#FF00FF
    constexpr Color WHITE = Color(1.f, 1.f, 1.f); ///< \#FFFFFF
    constexpr Color GRAY = Color(0.5f, 0.5f, 0.5f); ///< \#7F7F7F
    constexpr Color BLACK = Color(0.f, 0.f, 0.f); ///< \#000000
}

}

#endif // PGE_COLOR_H_INCLUDED

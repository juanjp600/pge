#ifndef PGE_COLOR_H_INCLUDED
#define PGE_COLOR_H_INCLUDED

#include <PGE/Types/Types.h>

namespace PGE {

/// An RGBA color represented via 4 floating point numbers.
/// Attributes are public.
class Color {
    public:
        /// Default constructs fully opaque white (#PGE::Color::WHITE).
        Color();
        /// Converts bytes in range [0, 255] to floats in range [0, 1].
        Color(byte r, byte g, byte b, byte a = 255);
        /// Values should be in range [0, 1].
        Color(float r, float g, float b, float a = 1.f);

        /// Converts HSV(A) to RGBA.
        /// @param[in] h The hue, must be in range [0, 360].
        /// @param[in] s The saturation, must be in range [0, 1].
        /// @param[in] v The value, must be in range [0, 1].
        /// @param[in] a The alpha, should be in range [0, 1].
        /// @exception #PGE::Exception If hue, saturation or value are outside their expected range.
        /// @see https://en.wikipedia.org/wiki/HSL_and_HSV
        static Color fromHSV(float h, float s, float v, float a = 1.f);

        /// Uses regular float equality.
        bool operator==(const Color& other) const;
        /// Uses regular float equality.
        bool operator!=(const Color& other) const;

        byte getRed() const; byte getGreen() const; byte getBlue() const; byte getAlpha() const;
        void setRed(byte r); void setGreen(byte g); void setBlue(byte b); void setAlpha(byte a);

        float red; float green; float blue; float alpha;

        /// A few common colors.
        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color ORANGE;
        static const Color YELLOW;
        static const Color CYAN;
        static const Color MAGENTA;
        static const Color WHITE;
        static const Color GRAY;
        static const Color BLACK;
};

}

#endif // PGE_COLOR_H_INCLUDED

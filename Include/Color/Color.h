#ifndef PGE_COLOR_H_INCLUDED
#define PGE_COLOR_H_INCLUDED

namespace PGE {

class Color {
    public:
        Color();
        Color(int r, int g, int b, int a = 255);
        Color(float r, float g, float b, float a = 1.f);

        static Color fromHSV(float h, float s, float v, float a = 1.f);

        bool operator==(const Color& other) const;
        bool operator!=(const Color& other) const;

        int getRedInt() const; int getGreenInt() const; int getBlueInt() const; int getAlphaInt() const;
        void setRedInt(int r); void setGreenInt(int g); void setBlueInt(int b); void setAlphaInt(int a);

        float red; float green; float blue; float alpha;

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

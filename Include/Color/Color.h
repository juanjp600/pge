#ifndef PGE_COLOR_H_INCLUDED
#define PGE_COLOR_H_INCLUDED

namespace PGE {

class Color {
    public:
        Color();
        Color(int r,int g,int b,int a=255);
        Color(float r,float g,float b,float a=1.0f);

        bool operator==(const Color& other) const;
        bool operator!=(const Color& other) const;

        int getRedInt() const; int getGreenInt() const; int getBlueInt() const; int getAlphaInt() const;
        void setRedInt(int r); void setGreenInt(int g); void setBlueInt(int b); void setAlphaInt(int a);

        float red; float green; float blue; float alpha;

        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Orange;
        static const Color Yellow;
        static const Color Cyan;
        static const Color Magenta;
        static const Color White;
        static const Color Gray;
        static const Color Black;
};

}

#endif

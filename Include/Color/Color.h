#ifndef PGE_COLOR_H_INCLUDED
#define PGE_COLOR_H_INCLUDED

namespace PGE {

class Color {
    public:
        Color();
        Color(int r,int g,int b,int a);
        Color(float r,float g,float b,float a);

        int getRedInt() const; int getGreenInt() const; int getBlueInt() const; int getAlphaInt() const;
        void setRedInt(int r); void setGreenInt(int g); void setBlueInt(int b); void setAlphaInt(int a);

        float red; float green; float blue; float alpha;
};

}

#endif

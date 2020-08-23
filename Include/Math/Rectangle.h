#ifndef PGE_RECTANGLE_H_INCLUDED
#define PGE_RECTANGLE_H_INCLUDED

#include "Vector.h"

namespace PGE {

class Rectanglef; class Rectanglei;

class Rectanglef {
    public:
        Rectanglef();
        Rectanglef(const Vector2f& itl,const Vector2f& ibr);
        Rectanglef(float il,float it,float ir,float ib);
        Rectanglef(const Rectanglei& ri);

        bool operator==(const Rectanglef& other) const;
        bool operator!=(const Rectanglef& other) const;

        void addPoint(float x,float y);
        void addPoint(const Vector2f& point);

        float width() const; float height() const; float area() const;
        Vector2f center() const;
        Vector2f topLeftCorner() const; Vector2f bottomRightCorner() const;
        Vector2f topRightCorner() const; Vector2f bottomLeftCorner() const;

        bool isPointInside(const Vector2f& p) const;
        bool intersects(const Rectanglef& other) const;

    private:
        Vector2f tl; Vector2f br;
        void repair();
};

class Rectanglei {
    public:
        Rectanglei();
        Rectanglei(const Vector2i& itl,const Vector2i& ibr);
        Rectanglei(int il,int it,int ir,int ib);
        Rectanglei(const Rectanglef& rf);

        void addPoint(int x,int y);
        void addPoint(const Vector2i& point);

        int width() const; int height() const; int area() const;
        Vector2i topLeftCorner() const; Vector2i bottomRightCorner() const;
        Vector2i topRightCorner() const; Vector2i bottomLeftCorner() const;

        bool isPointInside(const Vector2i& p) const;
        bool intersects(const Rectanglei& other) const;
    private:
        Vector2i tl; Vector2i br;
        void repair();
};

}

#endif

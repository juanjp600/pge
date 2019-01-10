#ifndef PGE_LINE_H_INCLUDED
#define PGE_LINE_H_INCLUDED

#include "Vector.h"
#include "Rectangle.h"

namespace PGE {

struct Line2f; struct Line2i;

struct Line2f {
    Vector2f pointA; Vector2f pointB;

    Line2f();
    Line2f(const Vector2f& a,const Vector2f& b);
    Line2f(float ax,float ay,float bx,float by);
    Line2f(const Line2i& li);

    Rectanglef boundingBox() const;
    bool intersects(const Line2f& other,Vector2f& point,bool segmentOnly=true) const;
    Vector2f closestPoint(const Vector2f& point,bool segmentOnly=true) const;
};

struct Line2i {
    Vector2i pointA; Vector2i pointB;

    Line2i();
    Line2i(const Vector2i& a,const Vector2i& b);
    Line2i(int ax,int ay,int bx,int by);
    Line2i(const Line2f& lf);

    Rectanglei boundingBox() const;
    bool intersects(const Line2i& other,Vector2f& point,bool segmentOnly=true) const;
};

}

#endif

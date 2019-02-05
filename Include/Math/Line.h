#ifndef PGE_LINE_H_INCLUDED
#define PGE_LINE_H_INCLUDED

#include "Vector.h"
#include "Rectangle.h"
#include "AABBox.h"

namespace PGE {

class Line2f; class Line2i; class Line3f;

class Line2f {
    public:
        Vector2f pointA; Vector2f pointB;

        Line2f();
        Line2f(const Vector2f& a,const Vector2f& b);
        Line2f(float ax,float ay,float bx,float by);
        Line2f(const Line2i& li);

        Rectanglef boundingBox() const;
        bool intersects(const Line2f& other,Vector2f& point,bool segmentOnly=true) const;
        Vector2f closestPoint(const Vector2f& point,bool segmentOnly=true) const;
};

class Line2i {
    public:
        Vector2i pointA; Vector2i pointB;

        Line2i();
        Line2i(const Vector2i& a,const Vector2i& b);
        Line2i(int ax,int ay,int bx,int by);
        Line2i(const Line2f& lf);

        Rectanglei boundingBox() const;
        bool intersects(const Line2i& other,Vector2f& point,bool segmentOnly=true) const;
};

class Line3f {
    public:
        Vector3f pointA; Vector3f pointB;

        Line3f();
        Line3f(const Vector3f& a,const Vector3f& b);
        Line3f(float ax,float ay,float az,float bx,float by,float bz);

        AABBox boundingBox() const;
        Vector3f closestPoint(const Vector3f& point,bool segmentOnly=true) const;
};

}

#endif

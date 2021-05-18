#include <stdlib.h>
#include <math.h>

#include <Math/Line.h>

using namespace PGE;

Line2f::Line2f() {
    pointA = Vector2f::ZERO; pointB = Vector2f::ZERO;
}

Line2f::Line2f(const Vector2f& a,const Vector2f& b) {
    pointA = a; pointB = b;
}

Line2f::Line2f(float ax,float ay,float bx,float by) {
    pointA = Vector2f(ax,ay); pointB = Vector2f(bx,by);
}

Line2f::Line2f(const Line2i& li) {
    pointA = Vector2f(li.pointA); pointB = Vector2f(li.pointB);
}

Rectanglef Line2f::boundingBox() const {
    return Rectanglef(pointA,pointB);
}

bool Line2f::intersects(const Line2f& other,Vector2f& point,bool segmentOnly) const {
    Vector2f p1a = pointA; Vector2f p1b = pointB;
    Vector2f p2a = other.pointA; Vector2f p2b = other.pointB;

    float denominator = ((p1a.x-p1b.x)*(p2a.y-p2b.y))-((p1a.y-p1b.y)*(p2a.x-p2b.x));
    //If denominator=0, then lines are parallel and cannot intersect.
    //Note: we consider coincident lines to not intersect.
    if (fabs(denominator)<0.001f) { return false; }

    point.x = (((p1a.x*p1b.y)-(p1a.y*p1b.x))*(p2a.x-p2b.x))-((p1a.x-p1b.x)*((p2a.x*p2b.y)-(p2a.y*p2b.x)));
    point.y = (((p1a.x*p1b.y)-(p1a.y*p1b.x))*(p2a.y-p2b.y))-((p1a.y-p1b.y)*((p2a.x*p2b.y)-(p2a.y*p2b.x)));
    point = point/denominator;

    if (!segmentOnly) { return true; }
    return boundingBox().isPointInside(point) && other.boundingBox().isPointInside(point);
}

Vector2f Line2f::closestPoint(const Vector2f& point,bool segmentOnly) const {
    Vector2f aToP = point - pointA;
    Vector2f aToB = pointB - pointA;

    float t = aToP.dotProduct(aToB)/aToB.lengthSquared();

    if (segmentOnly) {
        if (t<0) { return pointA; }
        if (t>1) { return pointB; }
    }
    return pointA + aToB * t;
}

Line2i::Line2i() {
    pointA = Vector2i::ZERO; pointB = Vector2i::ZERO;
}

Line2i::Line2i(const Vector2i& a,const Vector2i& b) {
    pointA = a; pointB = b;
}

Line2i::Line2i(int ax,int ay,int bx,int by) {
    pointA = Vector2i(ax,ay); pointB = Vector2i(bx,by);
}

Line2i::Line2i(const Line2f& li) {
    pointA = Vector2i(li.pointA); pointB = Vector2i(li.pointB);
}

Rectanglei Line2i::boundingBox() const {
    return Rectanglei(pointA,pointB);
}

bool Line2i::intersects(const Line2i& other,Vector2f& point,bool segmentOnly) const {
    Vector2i p1a = pointA; Vector2i p1b = pointB;
    Vector2i p2a = other.pointA; Vector2i p2b = other.pointB;

    float denominator = (float)(((p1a.x-p1b.x)*(p2a.y-p2b.y))-((p1a.y-p1b.y)*(p2a.x-p2b.x)));
    //If denominator=0, then lines are parallel and cannot intersect.
    //Note: we consider coincident lines to not intersect.
    if (fabs(denominator)<0.001f) { return false; }

    point.x = (float)((((p1a.x*p1b.y)-(p1a.y*p1b.x))*(p2a.x-p2b.x))-((p1a.x-p1b.x)*((p2a.x*p2b.y)-(p2a.y*p2b.x))));
    point.y = (float)((((p1a.x*p1b.y)-(p1a.y*p1b.x))*(p2a.y-p2b.y))-((p1a.y-p1b.y)*((p2a.x*p2b.y)-(p2a.y*p2b.x))));
    point = point/denominator;

    if (!segmentOnly) { return true; }
    return boundingBox().isPointInside(point) && other.boundingBox().isPointInside(point);
}

Line3f::Line3f() {
    pointA = Vector3f::ZERO; pointB = Vector3f::ZERO;
}

Line3f::Line3f(const Vector3f& a,const Vector3f& b) {
    pointA = a; pointB = b;
}

Line3f::Line3f(float ax,float ay,float az,float bx,float by,float bz) {
    pointA = Vector3f(ax,ay,az); pointB = Vector3f(bx,by,bz);
}

AABBox Line3f::boundingBox() const {
    return AABBox(pointA,pointB);
}

Vector3f Line3f::closestPoint(const Vector3f& point,bool segmentOnly) const {
    Vector3f aToP = point - pointA;
    Vector3f aToB = pointB - pointA;

    float t = aToP.dotProduct(aToB)/aToB.lengthSquared();

    if (segmentOnly) {
        if (t<0) { return pointA; }
        if (t>1) { return pointB; }
    }
    return pointA + aToB * t;
}

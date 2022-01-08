#ifndef PGE_LINE_H_INCLUDED
#define PGE_LINE_H_INCLUDED

#include "Rectangle.h"
#include "AABBox.h"

namespace PGE {

class Line2f {
    public:
        Vector2f pointA; Vector2f pointB;

        constexpr Line2f() = default;
        constexpr Line2f(const Vector2f& a, const Vector2f& b) : pointA(a), pointB(b) { }
        constexpr Line2f(float ax, float ay, float bx, float by) : pointA(ax, ay), pointB(bx, by) { }

        constexpr bool operator==(const Line2f& other) const {
            return pointA == other.pointA && pointB == other.pointB || pointA == other.pointB && pointB == other.pointA;
        }

        constexpr bool equals(const Line2f& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return pointA.equals(other.pointA, epsilon) && pointB.equals(other.pointB, epsilon)
                || pointA.equals(other.pointB, epsilon) && pointB.equals(other.pointA, epsilon);
        }

        constexpr Rectanglef boundingBox() const { return Rectanglef(pointA, pointB); }

        constexpr bool intersects(const Line2f& other, Vector2f& point, bool segmentOnly = true) const {
            Vector2f p1a(pointA); Vector2f p1b(pointB);
            Vector2f p2a(other.pointA); Vector2f p2b(other.pointB);

            float denominator = ((p1a.x - p1b.x) * (p2a.y - p2b.y)) - ((p1a.y - p1b.y) * (p2a.x - p2b.x));
            // If denominator = 0, then lines are parallel and cannot intersect.
            // Note: we consider coincident lines to not intersect.
            if (Math::equalFloats(denominator, 0.f)) { return false; }

            point.x = (((p1a.x * p1b.y) - (p1a.y * p1b.x)) * (p2a.x - p2b.x)) - ((p1a.x - p1b.x) * ((p2a.x * p2b.y) - (p2a.y * p2b.x)));
            point.y = (((p1a.x * p1b.y) - (p1a.y * p1b.x)) * (p2a.y - p2b.y)) - ((p1a.y - p1b.y) * ((p2a.x * p2b.y) - (p2a.y * p2b.x)));
            point = point / denominator;

            if (!segmentOnly) { return true; }
            return boundingBox().isPointInside(point) && other.boundingBox().isPointInside(point);
        }

        constexpr Vector2f closestPoint(const Vector2f& point, bool segmentOnly = true) const {
            Vector2f aToP = point - pointA;
            Vector2f aToB = pointB - pointA;

            float t = aToP.dotProduct(aToB) / aToB.lengthSquared();

            if (segmentOnly) {
                if (t < 0) { return pointA; }
                if (t > 1) { return pointB; }
            }
            return pointA + aToB * t;
        }
};
static_assert(sizeof(Line2f) == 2 * sizeof(Vector2f));

class Line3f {
    public:
        Vector3f pointA; Vector3f pointB;

        constexpr Line3f() = default;
        constexpr Line3f(const Vector3f& a, const Vector3f& b) : pointA(a), pointB(b) { }
        constexpr Line3f(float ax, float ay, float az, float bx, float by, float bz) : pointA(ax, ay, az), pointB(bx, by, bz) { }

        constexpr AABBox boundingBox() const { AABBox box(pointA); box.addPoint(pointB); return box; }

        constexpr Vector3f closestPoint(const Vector3f& point, bool segmentOnly = true) const {
            Vector3f aToP = point - pointA;
            Vector3f aToB = pointB - pointA;

            float t = aToP.dotProduct(aToB) / aToB.lengthSquared();

            if (segmentOnly) {
                if (t < 0) { return pointA; }
                if (t > 1) { return pointB; }
            }
            return pointA + aToB * t;
        }
};
static_assert(sizeof(Line3f) == 2 * sizeof(Vector3f));

class Line2i {
    public:
        Vector2i pointA; Vector2i pointB;

        constexpr Line2i() = default;
        constexpr Line2i(const Vector2i& a, const Vector2i& b) : pointA(a), pointB(b) { }
        constexpr Line2i(int ax, int ay, int bx, int by) : pointA(ax, ay), pointB(bx, by) { }
        constexpr explicit Line2i(const Line2f& lf) : pointA(lf.pointA), pointB(lf.pointB) { }

        constexpr operator const Line2f() const { return Line2f(pointA, pointB); }

        constexpr Rectanglei boundingBox() const { return Rectanglei(pointA, pointB); }

        constexpr bool intersects(const Line2i& other, Vector2f& point, bool segmentOnly = true) const {
            Vector2i p1a = pointA; Vector2i p1b = pointB;
            Vector2i p2a = other.pointA; Vector2i p2b = other.pointB;

            float denominator = (float)(((p1a.x - p1b.x) * (p2a.y - p2b.y)) - ((p1a.y - p1b.y) * (p2a.x - p2b.x)));
            // If denominator = 0, then lines are parallel and cannot intersect.
            // Note: we consider coincident lines to not intersect.
            if (Math::equalFloats(denominator, 0.f)) { return false; }

            point.x = (float)((((p1a.x * p1b.y) - (p1a.y * p1b.x)) * (p2a.x - p2b.x)) - ((p1a.x - p1b.x) * ((p2a.x * p2b.y) - (p2a.y * p2b.x))));
            point.y = (float)((((p1a.x * p1b.y) - (p1a.y * p1b.x)) * (p2a.y - p2b.y)) - ((p1a.y - p1b.y) * ((p2a.x * p2b.y) - (p2a.y * p2b.x))));
            point = point / denominator;

            if (!segmentOnly) { return true; }
            return boundingBox().isPointInside(Vector2i(point)) && other.boundingBox().isPointInside(Vector2i(point));
        }
};
static_assert(sizeof(Line2i) == 2 * sizeof(Vector2i));

}

#endif // PGE_LINE_H_INCLUDED

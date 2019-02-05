#ifndef PGE_PLANE_H_INCLUDED
#define PGE_PLANE_H_INCLUDED

#include "Vector.h"
#include "Line.h"

namespace PGE {

class Plane
{
    public:
        Vector3f normal;
        float distanceFromOrigin;

        Plane();
        Plane(Vector3f p1, Vector3f p2, Vector3f p3);
        Plane(Vector3f norm, Vector3f pointOnPlane);
        Plane(Vector3f norm, float distFromOrigin);

        int onPlane(Vector3f co, float epsilon = 0.5f);
        bool getIntersectionPoint(Line3f line, Vector3f& intersectionPoint, float& coveredAmount, bool ignoreDirection = false, bool ignoreSegment = false);
        float evalAtPoint(Vector3f co);
        bool equals(Plane other, float delta = 0.001f);

        static bool getPlaneIntersect(Plane p1, Plane p2, Plane p3, Vector3f& intersectionPoint);
};

}

#endif

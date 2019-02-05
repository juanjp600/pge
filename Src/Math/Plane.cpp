#include <Math/Plane.h>

#include <stdlib.h>
#include <math.h>

using namespace PGE;

Plane::Plane() {
    normal = Vector3f(0.f,1.f,0.f);
    distanceFromOrigin = 0.f;}

Plane::Plane(Vector3f p1, Vector3f p2, Vector3f p3) {
    Vector3f ab = p2.add(p1.multiply(-1.f));
    Vector3f ac = p3.add(p1.multiply(-1.f));

    normal = ac.crossProduct(ab).normalize();
    distanceFromOrigin = normal.dotProduct(p1);
}

Plane::Plane(Vector3f norm, Vector3f pointOnPlane) {
    normal = norm.normalize();
    distanceFromOrigin = normal.dotProduct(pointOnPlane);
}

Plane::Plane(Vector3f norm, float distFromOrigin) {
    normal = norm.normalize();
    distanceFromOrigin = distFromOrigin;
}

int Plane::onPlane(Vector3f co, float epsilon) {
    float res = evalAtPoint(co);
    if (fabs(res) < epsilon) { return 0; }
    if (res < 0) { return -1; }
    return 1;
}

bool Plane::getIntersectionPoint(Line3f line, Vector3f& intersectionPoint, float& coveredAmount, bool ignoreDirection, bool ignoreSegment) {
    // http://softsurfer.com/Archive/algorithm_0104/algorithm_0104B.htm#Line%20Intersections
    // http://paulbourke.net/geometry/planeline/

    Vector3f dir = line.pointB.add(line.pointA.multiply(-1.f));
    float denominator = -normal.dotProduct(dir);
    float numerator = normal.dotProduct(line.pointA.add(normal.multiply(-distanceFromOrigin)));
    if (fabs(denominator) < 0.001f || (!ignoreDirection && denominator < 0)) { return false; }
    float u = numerator / denominator;
    if (!ignoreSegment && (u < 0 || u > 1)) { return false; }
    intersectionPoint = line.pointA.add(dir.multiply(u));
    coveredAmount = u;
    return true;
}

float Plane::evalAtPoint(Vector3f co) {
    return normal.dotProduct(co) - distanceFromOrigin;
}

bool Plane::equals(Plane other, float delta) {
    return (normal.add(other.normal.multiply(-1.f)).lengthSquared()<delta*delta)
            && (fabs(distanceFromOrigin - other.distanceFromOrigin) < delta);
}

bool Plane::getPlaneIntersect(Plane p1, Plane p2, Plane p3, Vector3f& intersectionPoint) {
    // http://paulbourke.net/geometry/3planes/

    Vector3f c1 = p2.normal.crossProduct(p3.normal);
    Vector3f c2 = p3.normal.crossProduct(p1.normal);
    Vector3f c3 = p1.normal.crossProduct(p2.normal);

    float denom = p1.normal.dotProduct(c1);
    if (denom < 0.001f) { return false; } // No intersection, planes must be parallel

    Vector3f numer = (c1.multiply(p1.distanceFromOrigin)).add(c2.multiply(p2.distanceFromOrigin)).add(c3.multiply(p3.distanceFromOrigin));
    intersectionPoint = numer.multiply(1.f / denom);
    return true;
}

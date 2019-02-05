#include <Math/AABBox.h>
#include <cmath>

using namespace PGE;

AABBox::AABBox(const Vector3f& inPoint) {
    min = inPoint; max = inPoint;
}

AABBox::AABBox(const Vector3f& firstPoint,const Vector3f& secondPoint) {
    min = firstPoint; max = firstPoint;
    addPoint(secondPoint);
}

Vector3f AABBox::getMin() {
    return min;
}

Vector3f AABBox::getMax() {
    return max;
}

Vector3f AABBox::getCenter() {
    return min.add(max).multiply(0.5f);
}

Vector3f AABBox::getDims() {
    return max.subtract(min);
}

void AABBox::addPoint(const Vector3f& point) {
    min.x = fmin(min.x, point.x);
    min.y = fmin(min.y, point.y);
    min.z = fmin(min.z, point.z);

    max.x = fmax(max.x, point.x);
    max.y = fmax(max.y, point.y);
    max.z = fmax(max.z, point.z);
}

void AABBox::reset(const Vector3f& point) {
    min = point; max = point;
}


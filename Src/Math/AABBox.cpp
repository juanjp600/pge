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

Vector3f AABBox::getMin() const {
    return min;
}

Vector3f AABBox::getMax() const {
    return max;
}

Vector3f AABBox::getCenter() const {
    return min.add(max).multiply(0.5f);
}

Vector3f AABBox::getDims() const {
    return max.subtract(min);
}

bool AABBox::intersects(const AABBox& other) const {
    Vector3f otherMin = other.getMin();
    Vector3f otherMax = other.getMax();

    return otherMax.x>=min.x && otherMax.y>=min.y && otherMax.z>=min.z &&
           otherMin.x<=max.x && otherMin.y<=max.y && otherMin.z<=max.z;
}

void AABBox::addPoint(const Vector3f& point) {
    min.x = min.x < point.x ? min.x : point.x;
    min.y = min.y < point.y ? min.y : point.y;
    min.z = min.z < point.z ? min.z : point.z;

    max.x = max.x > point.x ? max.x : point.x;
    max.y = max.y > point.y ? max.y : point.y;
    max.z = max.z > point.z ? max.z : point.z;
}

void AABBox::reset(const Vector3f& point) {
    min = point; max = point;
}


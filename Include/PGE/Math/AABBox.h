#ifndef PGE_AABBOX_H_INCLUDED
#define PGE_AABBOX_H_INCLUDED


#include "Vector.h"

namespace PGE {

// Axis Aligned Bounding Box
class AABBox {
    private:
        Vector3f min; Vector3f max;

    public:
        constexpr AABBox() = default;
        constexpr AABBox(const Vector3f& inPoint) : min(inPoint), max(inPoint) { }

        constexpr bool operator==(const AABBox& other) const = default;

        constexpr bool equals(const AABBox& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return min.equals(other.min, epsilon) && max.equals(other.max, epsilon);
        }

        constexpr const Vector3f& getMin() const { return min; }
        constexpr const Vector3f& getMax() const { return max; }
        constexpr Vector3f getCenter() const { return (min + max) * 0.5f; }
        constexpr Vector3f getDims() const { return max - min; }

        constexpr bool contains(const Vector3f& point) const {
            return point.x >= min.x && point.y >= min.y && point.x <= max.x && point.y <= max.y;
        }

        constexpr bool intersects(const AABBox& other) const {
            Vector3f otherMin = other.getMin();
            Vector3f otherMax = other.getMax();

            return otherMax.x >= min.x && otherMax.y >= min.y && otherMax.z >= min.z
                && otherMin.x <= max.x && otherMin.y <= max.y && otherMin.z <= max.z;
        }

        constexpr void addPoint(const Vector3f& point) {
            min.x = min.x < point.x ? min.x : point.x;
            min.y = min.y < point.y ? min.y : point.y;
            min.z = min.z < point.z ? min.z : point.z;

            max.x = max.x > point.x ? max.x : point.x;
            max.y = max.y > point.y ? max.y : point.y;
            max.z = max.z > point.z ? max.z : point.z;
        }
};
static_assert(sizeof(AABBox) == 2 * sizeof(Vector3f));

}

#endif // PGE_AABBOX_H_INCLUDED

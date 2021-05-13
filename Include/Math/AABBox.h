#ifndef PGE_AABBOX_H_INCLUDED
#define PGE_AABBOX_H_INCLUDED

#include "Vector.h"

namespace PGE {

class AABBox {
    public:
        AABBox() = default;
        AABBox(const Vector3f& inPoint);
        AABBox(const Vector3f& firstPoint, const Vector3f& secondPoint);

        const Vector3f& getMin() const;
        const Vector3f& getMax() const;
        Vector3f getCenter() const;
        Vector3f getDims() const;

        bool intersects(const AABBox& other) const;

        void reset(const Vector3f& point);
        void addPoint(const Vector3f& point);
    private:
        Vector3f min; Vector3f max;
};

}

#endif // PGE_AABBOX_H_INCLUDED

#ifndef PGE_AABBOX_H_INCLUDED
#define PGE_AABBOX_H_INCLUDED

#include "Vector.h"

namespace PGE {

class AABBox {
    public:
        AABBox(const Vector3f& inPoint);
        AABBox(const Vector3f& firstPoint,const Vector3f& secondPoint);

        Vector3f getMin();
        Vector3f getMax();
        Vector3f getCenter();
        Vector3f getDims();

        void reset(const Vector3f& point);
        void addPoint(const Vector3f& point);
    private:
        Vector3f min; Vector3f max;
};

}

#endif

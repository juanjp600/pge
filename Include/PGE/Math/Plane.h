/*

    Ported from Sledge, see https://github.com/LogicAndTrick/sledge/
    Altered for use in pge

    BSD 3-Clause License

    Copyright (c) 2018, Daniel Walder
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef PGE_PLANE_H_INCLUDED
#define PGE_PLANE_H_INCLUDED

#include "Line.h"

namespace PGE {

class Plane : private NoHeap {
    public:
        Vector3f normal;
        float distanceFromOrigin;

        constexpr Plane() : normal(0.f, 1.f, 0.f), distanceFromOrigin(0.f) { }

        constexpr Plane(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3)
            : normal((p3 - p1).crossProduct(p2 - p1).normalize()),
              distanceFromOrigin(normal.dotProduct(p1)) { }

        constexpr Plane(const Vector3f& norm, const Vector3f& pointOnPlane)
            : normal(norm.normalize()),
              distanceFromOrigin(normal.dotProduct(pointOnPlane)) { }

        constexpr Plane(const Vector3f& norm, float distOrig)
            : normal(norm.normalize()),
              distanceFromOrigin(distOrig) { }

        constexpr bool operator==(const Plane& other) const {
            return normal == other.normal && distanceFromOrigin == other.distanceFromOrigin;
        }

        constexpr bool operator!=(const Plane& other) const {
            return normal != other.normal || distanceFromOrigin != other.distanceFromOrigin;
        }

        constexpr bool equals(const Plane& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return normal.equals(other.normal, epsilon) && Math::equalFloats(distanceFromOrigin, other.distanceFromOrigin, epsilon);
        }

        constexpr float evalAtPoint(const Vector3f& co) const {
            return normal.dotProduct(co) - distanceFromOrigin;
        }

        enum class PointRelation {
            ON,
            BELOW,
            ABOVE,
        };

        constexpr PointRelation onPlane(const Vector3f& co, float epsilon = Math::EPSILON_DEFAULT) const {
            float res = evalAtPoint(co);
            if (Math::equalFloats(epsilon, 0.f)) { return PointRelation::ON; }
            if (res < 0) { return PointRelation::BELOW; }
            return PointRelation::ABOVE;
        }
        
        constexpr bool intersects(const Line3f& line, Vector3f& intersectionPoint, float& coveredAmount, bool ignoreDirection = false, bool ignoreSegment = false) const {
            // http://softsurfer.com/Archive/algorithm_0104/algorithm_0104B.htm#Line%20Intersections
            // http://paulbourke.net/geometry/planeline/

            Vector3f dir = line.pointB - line.pointA;
            float denominator = -normal.dotProduct(dir);
            float numerator = normal.dotProduct(line.pointA + normal * -distanceFromOrigin);
            if (Math::equalFloats(denominator, 0.f) || (!ignoreDirection && denominator < 0)) { return false; }
            float u = numerator / denominator;
            if (!ignoreSegment && (u < 0 || u > 1)) { return false; }
            intersectionPoint = line.pointA + dir * u;
            coveredAmount = u;
            return true;
        }

        static constexpr bool intersect(const Plane& p1, const Plane& p2, const Plane& p3, Vector3f& intersectionPoint) {
            // http://paulbourke.net/geometry/3planes/

            Vector3f c1 = p2.normal.crossProduct(p3.normal);
            Vector3f c2 = p3.normal.crossProduct(p1.normal);
            Vector3f c3 = p1.normal.crossProduct(p2.normal);

            float denom = p1.normal.dotProduct(c1);
            // No intersection, planes must be parallel
            if (Math::equalFloats(denom, 0.f)) { return false; }

            Vector3f numer = c1 * p1.distanceFromOrigin + c2 * p2.distanceFromOrigin + c3 * p3.distanceFromOrigin;
            intersectionPoint = numer / denom;
            return true;
        }
};

}

#endif // PGE_MATRIX_H_INCLUDED

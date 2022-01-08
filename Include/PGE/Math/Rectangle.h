#ifndef PGE_RECTANGLE_H_INCLUDED
#define PGE_RECTANGLE_H_INCLUDED

#include "Vector.h"

namespace PGE {

class Rectanglef {
    private:
        Vector2f tl; Vector2f br;

        constexpr void repair() {
            if (tl.x > br.x) {
                float tlx = tl.x; float brx = br.x;
                br.x = tlx; tl.x = brx;
            }
            if (tl.y > br.y) {
                float tly = tl.y; float bry = br.y;
                br.y = tly; tl.y = bry;
            }
        }

    public:
        constexpr Rectanglef() = default;
        constexpr Rectanglef(const Vector2f& itl, const Vector2f& ibr) : tl(itl), br(ibr) { repair(); }
        constexpr Rectanglef(float il, float it, float ir, float ib) : tl(il, it), br(ir, ib) { repair(); }
        static constexpr Rectanglef fromDims(float x, float y, float width, float height) { return Rectanglef(x, y, x + width, y + height); }

        constexpr bool operator==(const Rectanglef& other) const = default;

        constexpr bool equals(const Rectanglef& other, float epsilon = Math::EPSILON_DEFAULT) const {
            return tl.equals(other.tl, epsilon) && br.equals(other.br, epsilon);
        }

        constexpr void addPoint(float x, float y) {
            if (x > br.x) { br.x = x; }
            else if (x < tl.x) { tl.x = x; }
            if (y > br.y) { br.y = y; }
            else if (y < tl.y) { tl.y = y; }
        }

        constexpr void addPoint(const Vector2f& point) {
            if (point.x > br.x) { br.x = point.x; }
            else if (point.x < tl.x) { tl.x = point.x; }
            if (point.y > br.y) { br.y = point.y; }
            else if (point.y < tl.y) { tl.y = point.y; }
        }

        constexpr float width() const { return br.x - tl.x; }
        constexpr float height() const { return br.y - tl.y; }
        constexpr float area() const { return width() * height(); }

        constexpr Vector2f center() const { return (tl + br) * 0.5f; }

        constexpr Vector2f topLeftCorner() const { return tl; }
        constexpr Vector2f bottomRightCorner() const { return br; }
        constexpr Vector2f topRightCorner() const { return Vector2f(br.x, tl.y); }
        constexpr Vector2f bottomLeftCorner() const { return Vector2f(tl.x, br.y); }

        constexpr bool isPointInside(const Vector2f& point) const {
            return point.x >= tl.x && point.y >= tl.y && point.x <= br.x && point.y <= br.y;
        }

        constexpr bool intersects(const Rectanglef& other) const {
            return other.tl.x <= tl.x && other.br.x >= tl.x && other.tl.y <= br.y && other.br.y >= tl.y;
        }
};
static_assert(sizeof(Rectanglef) == 2 * sizeof(Vector2f));

class Rectanglei {
    private:
        Vector2i tl; Vector2i br;

        constexpr void repair() {
            if (tl.x > br.x) {
                int tlx = tl.x; int brx = br.x;
                br.x = tlx; tl.x = brx;
            }
            if (tl.y > br.y) {
                int tly = tl.y; int bry = br.y;
                br.y = tly; tl.y = bry;
            }
        }

    public:
        constexpr Rectanglei() = default;
        constexpr Rectanglei(const Vector2i& itl, const Vector2i& ibr) : tl(itl), br(ibr) { repair(); }
        constexpr Rectanglei(int il, int it, int ir, int ib) : tl(il, it), br(ir, ib) { repair(); }
        static constexpr Rectanglei fromDims(int x, int y, int width, int height) { return Rectanglei(x, y, x + width, y + height); }

        constexpr bool operator==(const Rectanglei& other) const { return tl == other.tl && br == other.br; }
        constexpr bool operator!=(const Rectanglei& other) const { return tl != other.tl || br != other.br; }

        constexpr void addPoint(int x, int y) {
            if (x > br.x) { br.x = x; }
            else if (x < tl.x) { tl.x = x; }
            if (y > br.y) { br.y = y; }
            else if (y < tl.y) { tl.y = y; }
        }

        constexpr void addPoint(const Vector2i& point) {
            if (point.x > br.x) { br.x = point.x; }
            else if (point.x < tl.x) { tl.x = point.x; }
            if (point.y > br.y) { br.y = point.y; }
            else if (point.y < tl.y) { tl.y = point.y; }
        }

        constexpr int width() const { return br.x - tl.x; }
        constexpr int height() const { return br.y - tl.y; }
        constexpr int area() const { return width() * height(); }

        constexpr Vector2i topLeftCorner() const { return tl; }
        constexpr Vector2i bottomRightCorner() const { return br; }
        constexpr Vector2i topRightCorner() const { return Vector2i(br.x, tl.y); }
        constexpr Vector2i bottomLeftCorner() const { return Vector2i(tl.x, br.y); }

        constexpr bool isPointInside(const Vector2i& point) const {
            return point.x >= tl.x && point.y >= tl.y && point.x <= br.x && point.y <= br.y;
        }

        constexpr bool intersects(const Rectanglei& other) const {
            return other.tl.x <= tl.x && other.br.x >= tl.x && other.tl.y <= br.y && other.br.y >= tl.y;
        }
};
static_assert(sizeof(Rectanglei) == 2 * sizeof(Vector2i));

}

#endif // PGE_RECTANGLE_H_INCLUDED

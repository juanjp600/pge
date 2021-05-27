#include <PGE/Math/Rectangle.h>

using namespace PGE;

Rectanglef::Rectanglef() {
    tl = Vector2f::ZERO; br = Vector2f::ZERO;
}

Rectanglef::Rectanglef(const Vector2f& itl,const Vector2f& ibr) {
    tl = itl; br = ibr; repair();
}

Rectanglef::Rectanglef(float il,float it,float ir,float ib) {
    tl = Vector2f(il,it); br = Vector2f(ir,ib); repair();
}

Rectanglef::Rectanglef(const Rectanglei& ri) {
    tl = Vector2f(ri.topLeftCorner()); br = Vector2f(ri.bottomRightCorner());
}

bool Rectanglef::operator==(const Rectanglef& other) const {
    return topLeftCorner() == other.topLeftCorner() && bottomRightCorner() == other.bottomRightCorner();
}

bool Rectanglef::operator!=(const Rectanglef& other) const {
    return topLeftCorner() != other.topLeftCorner() || bottomRightCorner() != other.bottomRightCorner();
}

void Rectanglef::repair() {
    if (tl.x>br.x) {
        float tlx = tl.x; float brx = br.x;
        br.x = tlx; tl.x = brx;
    }
    if (tl.y>br.y) {
        float tly = tl.y; float bry = br.y;
        br.y = tly; tl.y = bry;
    }
}

void Rectanglef::addPoint(float x,float y) {
    if (x>br.x) { br.x = x; }
    else if (x<tl.x) { tl.x = x; }
    if (y>br.y) { br.y = y; }
    else if (y<tl.y) { tl.y = y; }
}

void Rectanglef::addPoint(const Vector2f& point) {
    if (point.x>br.x) { br.x = point.x; }
    else if (point.x<tl.x) { tl.x = point.x; }
    if (point.y>br.y) { br.y = point.y; }
    else if (point.y<tl.y) { tl.y = point.y; }
}

float Rectanglef::width() const {
    return br.x-tl.x;
}

float Rectanglef::height() const {
    return br.y-tl.y;
}

float Rectanglef::area() const {
    return width()*height();
}

Vector2f Rectanglef::center() const {
    return (tl + br) * 0.5f;
}

Vector2f Rectanglef::topLeftCorner() const {
    return tl;
}

Vector2f Rectanglef::bottomRightCorner() const {
    return br;
}

Vector2f Rectanglef::topRightCorner() const {
    return Vector2f(br.x,tl.y);
}

Vector2f Rectanglef::bottomLeftCorner() const {
    return Vector2f(tl.x,br.y);
}

bool Rectanglef::isPointInside(const Vector2f& point) const {
    if (point.x<tl.x) return false;
    if (point.y<tl.y) return false;
    if (point.x>br.x) return false;
    if (point.y>br.y) return false;
    return true;
}

bool Rectanglef::intersects(const Rectanglef& other) const {
    if (other.topLeftCorner().x>bottomRightCorner().x) return false;
    if (other.bottomRightCorner().x<topLeftCorner().x) return false;
    if (other.topLeftCorner().y>bottomRightCorner().y) return false;
    if (other.bottomRightCorner().y<topLeftCorner().y) return false;
    return true;
}

Rectanglei::Rectanglei() {
    tl = Vector2i::ZERO; br = Vector2i::ZERO;
}

Rectanglei::Rectanglei(const Vector2i& itl,const Vector2i& ibr) {
    tl = itl; br = ibr; repair();
}

Rectanglei::Rectanglei(int il,int it,int ir,int ib) {
    tl = Vector2i(il,it); br = Vector2i(ir,ib); repair();
}

Rectanglei::Rectanglei(const Rectanglef& rf) {
    tl = Vector2i(rf.topLeftCorner()); br = Vector2i(rf.bottomRightCorner());
}

bool Rectanglei::operator==(const Rectanglei& other) const {
    return topLeftCorner() == other.topLeftCorner() && bottomRightCorner() == other.bottomRightCorner();
}

bool Rectanglei::operator!=(const Rectanglei& other) const {
    return topLeftCorner() != other.topLeftCorner() || bottomRightCorner() != other.bottomRightCorner();
}

void Rectanglei::repair() {
    if (tl.x>br.x) {
        int tlx = tl.x; int brx = br.x;
        br.x = tlx; tl.x = brx;
    }
    if (tl.y>br.y) {
        int tly = tl.y; int bry = br.y;
        br.y = tly; tl.y = bry;
    }
}

void Rectanglei::addPoint(int x,int y) {
    if (x>br.x) { br.x = x; }
    else if (x<tl.x) { tl.x = x; }
    if (y>br.y) { br.y = y; }
    else if (y<tl.y) { tl.y = y; }
}

void Rectanglei::addPoint(const Vector2i& point) {
    if (point.x>br.x) { br.x = point.x; }
    else if (point.x<tl.x) { tl.x = point.x; }
    if (point.y>br.y) { br.y = point.y; }
    else if (point.y<tl.y) { tl.y = point.y; }
}

int Rectanglei::width() const {
    return br.x-tl.x;
}

int Rectanglei::height() const {
    return br.y-tl.y;
}

int Rectanglei::area() const {
    return width()*height();
}

Vector2i Rectanglei::topLeftCorner() const {
    return tl;
}

Vector2i Rectanglei::bottomRightCorner() const {
    return br;
}

Vector2i Rectanglei::topRightCorner() const {
    return Vector2i(br.x,tl.y);
}

Vector2i Rectanglei::bottomLeftCorner() const {
    return Vector2i(tl.x,br.y);
}

bool Rectanglei::isPointInside(const Vector2i& point) const {
    if (point.x<tl.x) return false;
    if (point.y<tl.y) return false;
    if (point.x>br.x) return false;
    if (point.y>br.y) return false;
    return true;
}

bool Rectanglei::intersects(const Rectanglei& other) const {
    if (other.topLeftCorner().x>bottomRightCorner().x) return false;
    if (other.bottomRightCorner().x<topLeftCorner().x) return false;
    if (other.topLeftCorner().y>bottomRightCorner().y) return false;
    if (other.bottomRightCorner().y<topLeftCorner().y) return false;
    return true;
}

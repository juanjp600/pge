#include <Mesh/Mesh.h>

using namespace PGE;

Vertex::Vertex(const Vector3f& p, const Vector3f& n, const std::vector<Vector2f>& tc, const Color& c) {
    pos = p; normal = n; uv = tc; color = c;
}

Vertex::Vertex(const Vector3f& p, const Vector3f& n, const Vector2f& tc, const Color& c) {
    pos = p; normal = n; uv.push_back(tc); color = c;
}

Primitive::Primitive(long ia,long ib) {
    a = ia; b = ib; c = -1;
}

Primitive::Primitive(long ia,long ib,long ic) {
    a = ia; b = ib; c = ic;
}

int Mesh::addVertex(const Vertex& v) {
    isDirty = true;
    vertices.push_back(v);
    return vertices.size()-1;
}

void Mesh::removeVertex(int v) {
    isDirty = true;
    vertices.erase(vertices.begin()+v);
    for (int i=0;i<primitives.size();i++) {
        bool shouldRemove = primitives[i].a==v || primitives[i].b==v;
        if (primitiveType==Primitive::TYPE::TRIANGLE) {
            shouldRemove |= primitives[i].c==v;
        }
        if (shouldRemove) {
            primitives.erase(primitives.begin()+i); i--;
            continue;
        }
        if (primitives[i].a>v) { primitives[i].a--; }
        if (primitives[i].b>v) { primitives[i].b--; }
        if (primitiveType==Primitive::TYPE::TRIANGLE) {
            if (primitives[i].c>v) { primitives[i].c--; }
        }
    }
}

void Mesh::clear() {
    isDirty = true;
    vertices.clear();
    primitives.clear();
}

int Mesh::addPrimitive(const Primitive& t) {
    isDirty = true;
    primitives.push_back(t);
    return primitives.size()-1;
}

void Mesh::removePrimitive(int t) {
    isDirty = true;
    primitives.erase(primitives.begin()+t);
}

void Mesh::setMaterial(Material* m) {
    isDirty = true;
    material = m;
}

const std::vector<Vertex>& Mesh::getVertices() const {
    return vertices;
}

const std::vector<Primitive>& Mesh::getPrimitives() const {
    return primitives;
}

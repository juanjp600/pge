#ifndef PGE_MESH_H_INCLUDED
#define PGE_MESH_H_INCLUDED

#include <Graphics/Graphics.h>

#include <Material/Material.h>

#include <Color/Color.h>
#include <Math/Vector.h>
#include <Math/Matrix.h>

#include <vector>

namespace PGE {

struct Vertex {
    Vertex(const Vector3f& p, const Vector3f& n, const std::vector<Vector2f>& tc, const Color& c);
    Vertex(const Vector3f& p, const Vector3f& n, const Vector2f& tc, const Color& c);

    Vector3f pos;
    Vector3f normal;
    std::vector<Vector2f> uv;
    Color color;
};

struct Primitive {
    enum class TYPE {
        TRIANGLE,
        LINE,
    };

    Primitive(long ia,long ib);
    Primitive(long ia,long ib,long ic);

    long a; long b; long c;
};

class Mesh {
    public:
        static Mesh* create(Graphics* gfx, Primitive::TYPE pt);
        Mesh* clone();
        virtual ~Mesh(){}

        int addVertex(const Vertex& v);
        void removeVertex(int v);
        int addPrimitive(const Primitive& t);
        void removePrimitive(int t);
        void clear();
        void setMaterial(Material* m);

        const std::vector<Vertex>& getVertices() const;
        const std::vector<Primitive>& getPrimitives() const;
        bool isOpaque() const;

        virtual void render() =0;
    protected:
        Mesh(){};

        bool opaque;
        bool isDirty = true;

        virtual void updateInternalData() =0;

        Material* material;

        Graphics* graphics;
        Primitive::TYPE primitiveType;

        std::vector<Vertex> vertices;
        std::vector<Primitive> primitives;
};

}

#endif

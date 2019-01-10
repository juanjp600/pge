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
    Vertex(const Vector3f& p,const Vector3f& n,const Vector2f& tc,const Color& c);

    Vector3f pos;
    Vector3f normal;
    Vector2f uv; //maybe have a second set of uv coords?
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
        virtual ~Mesh(){}

        int addVertex(const Vertex& v);
        void removeVertex(int v);
        int addPrimitive(const Primitive& t);
        void removePrimitive(int t);
        void clear();
        void setMaterial(Material* m);

        Matrix4x4f worldMatrix;

        const std::vector<Vertex>& getVertices() const;
        const std::vector<Primitive>& getPrimitives() const;

        virtual void render() =0;
    protected:
        Mesh(){};

        bool isDirty = true;

        virtual void updateInternalData() =0;

        Material* material;

        Primitive::TYPE primitiveType;

        std::vector<Vertex> vertices;
        std::vector<Primitive> primitives;
};

}

#endif

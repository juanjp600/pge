#ifndef PGE_MESH_H_INCLUDED
#define PGE_MESH_H_INCLUDED

#include <unordered_map>

#include <String/Key.h>

#include <Misc/Byte.h>
#include <Material/Material.h>
#include <Color/Color.h>
#include <Math/Vector.h>
#include <Math/Matrix.h>

namespace PGE {

class Graphics;

class Vertex {
    public:
        Vertex() = default;

        struct Property {
            enum class Type {
                FLOAT,
                UINT,
                VECTOR2F,
                VECTOR3F,
                VECTOR4F,
                COLOR,
                INVALID
            };

            Property() = default;
            Property(const Property& other);
            Type type = Type::INVALID;
            // TODO: Replace all this with set byte layout.
            union {
                float floatVal;
                unsigned int uintVal;
                Vector2f vector2fVal;
                Vector3f vector3fVal;
                Vector4f vector4fVal = Vector4f::ZERO;
                Color colorVal;
            } value;
        };

        const Property& getProperty(const String& name) const;
        
        void setFloat(const String& name,float val);
        void setUInt(const String& name,unsigned int val);
        void setVector2f(const String& name, const Vector2f& val);
        void setVector3f(const String& name, const Vector3f& val);
        void setVector4f(const String& name, const Vector4f& val);
        void setColor(const String& name, const Color& val);

    private:
        std::unordered_map<String::Key, Property> properties;
};

class Primitive {
    public:
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
        virtual ~Mesh() = default;

        void setGeometry(int vertexCount, const std::vector<Vertex>& verts, int primCount, const std::vector<Primitive>& prims);
        void setGeometry(const std::vector<Vertex>& verts, const std::vector<Primitive>& prims);
        void clearGeometry();
        void setMaterial(Material* m);

        const std::vector<Vertex>& getVertices(int& vertCount) const;
        const std::vector<Primitive>& getPrimitives(int& primCount) const;
        bool isOpaque() const;

        // Must be threadsafe.
        virtual void updateInternalData() = 0;

        virtual void render() = 0;

    protected:
        bool opaque;
        bool mustUpdateInternalData = true;
        bool mustReuploadInternalData = true;

        // Doesn't have to be threadsafe.
        // The idea is to update a large mesh in parallel and then only doing a low cost upload on the main thread.
        virtual void uploadInternalData() = 0;

        Material* material;

        Graphics* graphics;
        Primitive::TYPE primitiveType;

        int vertexCount; int primitiveCount;
        std::vector<Vertex> vertices;
        std::vector<Primitive> primitives;
};

}

#endif // PGE_MESH_H_INCLUDED

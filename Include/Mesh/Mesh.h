#ifndef PGE_MESH_H_INCLUDED
#define PGE_MESH_H_INCLUDED

#include <vector>

#include <Material/Material.h>
#include <Color/Color.h>
#include <Math/Vector.h>
#include <Math/Matrix.h>

namespace PGE {

class Graphics;

class Vertex {
    public:
        Vertex();
        Vertex(const Vertex& other);
        Vertex& operator=(const Vertex& other);
        enum class PROPERTY_TYPE {
            FLOAT,
            UINT,
            VECTOR2F,
            VECTOR3F,
            VECTOR4F,
            COLOR
        };

        struct Property {
            Property();
            Property(const Property& other);
            void copyOtherValue(const Property& other);
            long long hashCode;
            PROPERTY_TYPE type;
            union Value {
                Value();
                Value(const Value& other) =delete;
                float floatVal;
                unsigned int uintVal;
                Vector2f vector2fVal;
                Vector3f vector3fVal;
                Vector4f vector4fVal;
                Color colorVal;
            } value;
            const static Property def;
        };
        const Property& getProperty(const String& name,int& indexHint);
        void setFloat(const String& name,float val);
        void setUInt(const String& name,unsigned int val);
        void setVector2f(const String& name,Vector2f val);
        void setVector3f(const String& name,Vector3f val);
        void setVector4f(const String& name,Vector4f val);
        void setColor(const String& name,Color val);

    private:
        std::vector<Property> properties;
        Property& insertProperty(long long hashCode);
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
        virtual ~Mesh(){}

        void setGeometry(int vertexCount, const std::vector<Vertex>& verts, int primCount, const std::vector<Primitive>& prims);
        void setGeometry(const std::vector<Vertex>& verts, const std::vector<Primitive>& prims);
        void clearGeometry();
        void setMaterial(Material* m);

        const std::vector<Vertex>& getVertices(int& vertCount) const;
        const std::vector<Primitive>& getPrimitives(int& primCount) const;
        bool isOpaque() const;

        virtual void updateInternalData() = 0;

        virtual void render() = 0;

    protected:
        Mesh(){}
        
        virtual void cleanup() = 0;
        virtual void throwException(String func, String details) = 0;

        bool opaque;
        bool mustUpdateInternalData = true;
        bool mustReuploadInternalData = true;

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

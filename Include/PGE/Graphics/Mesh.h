#ifndef PGE_MESH_H_INCLUDED
#define PGE_MESH_H_INCLUDED

#include <unordered_map>

#include <PGE/String/Key.h>
#include <PGE/Types/Types.h>
#include <PGE/Color/Color.h>
#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Graphics/Material.h>

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
                INVALID,
            };

            Property() = default;
            Property(const Property& other);
            Type type = Type::INVALID;
            // TODO: Replace all this with set byte layout.
            union Value {
                float floatVal;
                unsigned uintVal;
                Vector2f vector2fVal;
                Vector3f vector3fVal;
                Vector4f vector4fVal;
                Color colorVal;
                
                Value();
            };
            Value value;
        };

        const Property& getProperty(const String& name) const;
        
        void setFloat(const String& name,float val);
        void setUInt(const String& name,unsigned val);
        void setVector2f(const String& name, const Vector2f& val);
        void setVector3f(const String& name, const Vector3f& val);
        void setVector4f(const String& name, const Vector4f& val);
        void setColor(const String& name, const Color& val);

    private:
        std::unordered_map<String::Key, Property> properties;
};

class Primitive {
    public:
        enum class Type {
            TRIANGLE,
            LINE
        };

        Primitive() = default;
        Primitive(long ia,long ib);
        Primitive(long ia,long ib,long ic);

        long a; long b; long c;
};

class Mesh {
    public:
        static Mesh* create(Graphics* gfx, Primitive::Type pt);
        Mesh* clone();
        virtual ~Mesh() = default;

        void setGeometry(const std::vector<Vertex>& verts, const std::vector<Primitive>& prims);
        // TODO: Possibly delegate this to the implementations.
        // In general it is worth investigating if we can optimize changes to meshes by offloading more work to the implementations.
        // Also e.g. encapsulating update and upload methods a bit would make for a better design. 
        void addGeometry(const std::vector<Vertex>& verts, const std::vector<Primitive>& prims);
        void clearGeometry();
        const std::vector<Vertex>& getVertices() const;
        const std::vector<Primitive>& getPrimitives() const;
        void setMaterial(Material* m);
        Material* getMaterial() const;

        bool isOpaque() const;

        // Must be threadsafe.
        virtual void updateInternalData() = 0;

        virtual void render() = 0;

    protected:
        bool mustUpdateInternalData = true;
        bool mustReuploadInternalData = true;

        // Doesn't have to be threadsafe.
        // The idea is to update a large mesh in parallel and then only doing a low cost upload on the main thread.
        virtual void uploadInternalData() = 0;

        Material* material;

        Graphics* graphics;
        Primitive::Type primitiveType;

        std::vector<Vertex> vertices;
        std::vector<Primitive> primitives;
};

}

#endif // PGE_MESH_H_INCLUDED

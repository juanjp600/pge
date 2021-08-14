#ifndef PGE_MESH_H_INCLUDED
#define PGE_MESH_H_INCLUDED

#include <unordered_map>
#include <optional>

#include <PGE/String/Key.h>
#include <PGE/Types/Types.h>
#include <PGE/Color/Color.h>
#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/StructuredData/StructuredData.h>
#include <PGE/ResourceManagement/PolymorphicHeap.h>

namespace PGE {

class Mesh : private PolymorphicHeap {
    public:
        class Material {
            public:
                enum class Opaque {
                    YES,
                    NO
                };

                Material();
                Material(class Shader& sh, Opaque o);
                Material(Shader& sh, class Texture& tex, Opaque o);
                Material(Shader& sh, const ReferenceVector<Texture>& texs, Opaque o);

                bool isValid() const;
                bool isOpaque() const;
                Shader& getShader() const;
                int getTextureCount() const;
                Texture& getTexture(int index) const;
            private:
                Shader* shader;
                ReferenceVector<Texture> textures;
                Opaque opaque;
        };

        enum class PrimitiveType {
            LINE,
            TRIANGLE,
        };

        struct Line {
            Line(u32 a, u32 b);

            u32 indices[2];
        };

        struct Triangle {
            Triangle(u32 a, u32 b, u32 c);

            u32 indices[3];
        };

        static Mesh* create(class Graphics& gfx);

        void setGeometry(StructuredData&& verts, const std::vector<Line>& lines);
        void setGeometry(StructuredData&& verts, const std::vector<Triangle>& triangles);
        void setGeometry(StructuredData&& verts, PrimitiveType type, std::vector<u32>&& inds);
        void clearGeometry();

        void setMaterial(const Material& m);

        bool isOpaque() const;

        void render();

    protected:
        virtual void uploadInternalData() = 0;
        virtual void renderInternal() = 0;

        Material material;

        std::optional<PrimitiveType> primitiveType;
        StructuredData vertices;
        std::vector<u32> indices;

    private:
        bool mustReuploadInternalData = true;
};

}

#endif // PGE_MESH_H_INCLUDED

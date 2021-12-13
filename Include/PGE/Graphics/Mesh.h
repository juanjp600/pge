#ifndef PGE_MESH_H_INCLUDED
#define PGE_MESH_H_INCLUDED

#include <optional>
#include <span>

#include <PGE/Types/Types.h>
#include <PGE/StructuredData/StructuredData.h>
#include <PGE/Types/PolymorphicHeap.h>

namespace PGE {

class Material;

enum class PrimitiveType {
    LINE,
    TRIANGLE,
};

class Mesh : private PolymorphicHeap {
    public:
        enum class UpdateStrategy {
            STATIC,
            DYNAMIC,
            PER_FRAME,
        };

        struct Line {
            Line(u32 a, u32 b);

            u32 indices[2];
        };
        static_assert(sizeof(Line) == 2 * sizeof(u32));

        struct Triangle {
            Triangle(u32 a, u32 b, u32 c);

            u32 indices[3];
        };
        static_assert(sizeof(Triangle) == 3 * sizeof(u32));

        static Mesh* create(class Graphics& gfx);

        void setGeometry(StructuredData&& verts, const std::span<Line>& primitives) {
            setGeometryInternal<2, PrimitiveType::LINE>(std::move(verts), primitives);
        }

        void setGeometry(StructuredData&& verts, const std::span<Triangle>& primitives) {
            setGeometryInternal<3, PrimitiveType::TRIANGLE>(std::move(verts), primitives);
        }

        void setGeometry(StructuredData&& verts, PrimitiveType type, std::vector<u32>&& inds);
        void clearGeometry();

        void setMaterial(Material* m);

        void setUpdateStrategy(UpdateStrategy us);

        bool isOpaque() const;

        void render();

    protected:
        virtual void uploadInternalData() = 0;
        virtual void renderInternal() = 0;

        Material* material = nullptr;

        UpdateStrategy strategy = UpdateStrategy::STATIC;

        std::optional<PrimitiveType> primitiveType;
        StructuredData vertices;
        std::vector<u32> indices;

    private:
        bool mustReuploadInternalData = true;

        template <byte VERTS_PER_PRIM, PrimitiveType PRIM_TYPE, OneOf<Line, Triangle> PRIMITIVE>
        void setGeometryInternal(StructuredData&& verts, const std::span<PRIMITIVE>& primitives) {
            assertMaterialLayout(verts);

            vertices = std::move(verts);
            indices.clear();
            indices.resize(primitives.size() * VERTS_PER_PRIM);
            memcpy(indices.data(), primitives.data(), primitives.size_bytes());
            primitiveType = PRIM_TYPE;

            mustReuploadInternalData = true;
        }

        void assertMaterialLayout(const StructuredData& verts, const std::source_location& location = std::source_location::current());
};

}

#endif // PGE_MESH_H_INCLUDED

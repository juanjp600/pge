#ifndef PGE_MESH_H_INCLUDED
#define PGE_MESH_H_INCLUDED

#include <optional>

#include <PGE/Types/Types.h>
#include <PGE/StructuredData/StructuredData.h>
#include <PGE/ResourceManagement/PolymorphicHeap.h>

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

        struct Triangle {
            Triangle(u32 a, u32 b, u32 c);

            u32 indices[3];
        };

        static Mesh* create(class Graphics& gfx);

        void setGeometry(StructuredData&& verts, const std::vector<Line>& lines);
        void setGeometry(StructuredData&& verts, const std::vector<Triangle>& triangles);
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
};

}

#endif // PGE_MESH_H_INCLUDED

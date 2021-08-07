#ifndef PGE_MESH_H_INCLUDED
#define PGE_MESH_H_INCLUDED

#include <unordered_map>

#include <PGE/String/Key.h>
#include <PGE/Types/Types.h>
#include <PGE/Color/Color.h>
#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Graphics/Material.h>
#include <PGE/StructuredData/StructuredData.h>
#include <PGE/ResourceManagement/PolymorphicHeap.h>

namespace PGE {

class Graphics;

class Mesh : private PolymorphicHeap {
    public:
        enum class PreserveGeometry {
            YES,
            NO,
        };

        enum class PrimitiveType {
            NONE,
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

        static Mesh* create(Graphics& gfx);
        Mesh* clone(Graphics& gfx);

        void setGeometry(const StructuredData& verts, const std::vector<Line>& lines);
        void setGeometry(const StructuredData& verts, const std::vector<Triangle>& triangles);
        void setGeometry(const StructuredData& verts, PrimitiveType type, std::vector<u32>&& inds);
        void clearGeometry();

        void setMaterial(Material* m, PreserveGeometry preserveGeometry);

        bool isOpaque() const;

        virtual void render() = 0;

    protected:
        bool mustReuploadInternalData = true;

        virtual void uploadInternalData() = 0;

        Material* material = nullptr;

        PrimitiveType primitiveType = PrimitiveType::NONE;
        StructuredData vertices;
        std::vector<u32> indices;
};

}

#endif // PGE_MESH_H_INCLUDED

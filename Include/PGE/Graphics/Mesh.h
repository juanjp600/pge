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

namespace PGE {

class Graphics;

class Mesh {
    public:
        enum class PrimitiveType {
            NONE,
            LINE,
            TRIANGLE
        };

        enum class PreserveGeometry {
            YES,
            NO
        };

        struct Line {
            Line(int a, int b);

            int indices[2];
        };

        struct Triangle {
            Triangle(int a, int b, int c);

            int indices[3];
        };

        static Mesh* create(Graphics* gfx);
        static Mesh* create(Graphics* gfx, Material* material, const StructuredData& verts, const std::vector<Line>& lines);
        static Mesh* create(Graphics* gfx, Material* material, const StructuredData& verts, const std::vector<Triangle>& triangles);
        Mesh* clone();
        virtual ~Mesh() = default;

        void setGeometry(const StructuredData& verts, const std::vector<Line>& lines);
        void setGeometry(const StructuredData& verts, const std::vector<Triangle>& triangles);
        void clearGeometry();

        void setMaterial(Material* m, PreserveGeometry preserveGeometry);

        bool isOpaque() const;

        virtual void render() = 0;

    protected:
        bool mustReuploadInternalData = true;

        virtual void uploadInternalData() = 0;

        Material* material = nullptr;
        Graphics* graphics = nullptr;

        PrimitiveType primitiveType;
        StructuredData vertices;
        std::vector<u32> indices;
};

}

#endif // PGE_MESH_H_INCLUDED

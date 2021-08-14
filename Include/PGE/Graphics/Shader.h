#ifndef PGE_SHADER_H_INCLUDED
#define PGE_SHADER_H_INCLUDED

#include <PGE/File/FilePath.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Math/Vector.h>
#include <PGE/Color/Color.h>
#include <PGE/StructuredData/StructuredData.h>
#include <PGE/ResourceManagement/PolymorphicHeap.h>

namespace PGE {

class Shader : private PolymorphicHeap {
    public:
        static Shader* load(class Graphics& gfx, const FilePath& path);
        virtual ~Shader() = default;

        const StructuredData::ElemLayout& getVertexLayout() { return vertexLayout; }

        // This is not heap-only to allow it as a map value.
        class Constant {
            public:
                virtual void setValue(const Matrix4x4f& value) = 0;
                virtual void setValue(const Vector2f& value) = 0;
                virtual void setValue(const Vector3f& value) = 0;
                virtual void setValue(const Vector4f& value) = 0;
                virtual void setValue(const Color& value) = 0;
                virtual void setValue(float value) = 0;
                virtual void setValue(u32 value) = 0;

            protected:
                Constant() = default;
                virtual ~Constant() = default;
        };
        virtual Constant& getVertexShaderConstant(const String& constName) = 0;
        virtual Constant& getFragmentShaderConstant(const String& constName) = 0;

    protected:
        Shader(const FilePath& path) : filepath(path) { }

        StructuredData::ElemLayout vertexLayout;
        const FilePath filepath;
};

}

#endif // PGE_SHADER_H_INCLUDED

#ifndef PGE_SHADER_H_INCLUDED
#define PGE_SHADER_H_INCLUDED

#include <span>

#include <PGE/File/FilePath.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Math/Vector.h>
#include <PGE/Color/Color.h>
#include <PGE/StructuredData/StructuredData.h>
#include <PGE/Types/PolymorphicHeap.h>

namespace PGE {

class Shader : private PolymorphicHeap {
    public:
        static Shader* load(class Graphics& gfx, const FilePath& path);
        virtual ~Shader() = default;

        const StructuredData::ElemLayout& getVertexLayout() { return vertexLayout; }

        // This is not heap-only to allow it as a map value.
        class Constant {
            public:
                void setValue(const StructuredType auto& value) { setValueInternal(std::span<byte>((byte*)&value, sizeof(value))); }

            protected:
                Constant() = default;
                virtual ~Constant() = default;

                virtual void setValueInternal(const std::span<byte>& data) = 0;
        };
        virtual Constant* getVertexShaderConstant(const String& constName) = 0;
        virtual Constant* getFragmentShaderConstant(const String& constName) = 0;

        int getTextureCount() const { PGE_ASSERT(textureCount != -1, "Texture count has not been initialized"); return textureCount; }

    protected:
        Shader(const FilePath& path) : filepath(path) { }

        StructuredData::ElemLayout vertexLayout;
        const FilePath filepath;
        int textureCount = -1;
};

}

#endif // PGE_SHADER_H_INCLUDED

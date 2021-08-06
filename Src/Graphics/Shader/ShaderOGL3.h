#ifndef PGEINTERNAL_SHADEROGL3_H_INCLUDED
#define PGEINTERNAL_SHADEROGL3_H_INCLUDED

#include <PGE/Graphics/Graphics.h>
#include <PGE/Graphics/Shader.h>
#include <PGE/String/String.h>
#include <PGE/String/Key.h>
#include <PGE/Math/Matrix.h>

#include "../../ResourceManagement/OGL3.h"
#include "../../ResourceManagement/ResourceManagerOGL3.h"

namespace PGE {

class ShaderOGL3 : public Shader, private GraphicsReferencer<class GraphicsOGL3> {
    public:
        ShaderOGL3(Graphics& gfx, const FilePath& path);

        Constant& getVertexShaderConstant(const String& name) override;
        Constant& getFragmentShaderConstant(const String& name) override;

        void useShader();
        void unbindGLAttribs();
    private:
        class ConstantOGL3 : public Constant, private GraphicsReferencer<GraphicsOGL3> {
            public:
                ConstantOGL3(GraphicsOGL3& gfx, GLint glLoc, GLenum glTyp, int glArrSz, StructuredData& data, const String::Key& dk);

                void setValue(const Matrix4x4f& value) override;
                void setValue(const Vector2f& value) override;
                void setValue(const Vector3f& value) override;
                void setValue(const Vector4f& value) override;
                void setValue(const Color& value) override;
                void setValue(float value) override;
                void setValue(u32 value) override;

                void setUniform();

            private:
                GLint glLocation;
                GLenum glType;
                int glArraySize;
                StructuredData& dataBuffer;
                String::Key dataKey;
        };

        std::unordered_map<String::Key, ConstantOGL3> vertexShaderConstants;
        std::unordered_map<String::Key, ConstantOGL3> fragmentShaderConstants;
        std::unordered_map<String::Key, ConstantOGL3> samplerConstants;

        struct GlAttribLocation {
            GlAttribLocation(GLint loc, GLenum elemType, int elemCount);

            GLint location;
            GLenum elementType;
            int elementCount;
        };

        std::unordered_map<String::Key, GlAttribLocation> glVertexAttribLocations;
        StructuredData::ElemLayout vertexLayout;

        StructuredData vertexUniformData;
        StructuredData fragmentUniformData;

        int glSizeToByteSize(GLenum type, int size) const;
        GLenum parsedTypeToGlType(const String& parsedType);
        void decomposeGlType(GLenum compositeType, GLenum& elemType, int& elemCount);

        void extractVertexUniforms(const String& vertexSource);
        void extractVertexAttributes(const String& vertexSource);
        void extractFragmentUniforms(const String& fragmentSource);
        void extractFragmentOutputs(const String fragmentSource);

        struct ParsedShaderVar {
            String name;
            String type;
        };
        void extractShaderVars(const String& src,const String& varKind,std::vector<ParsedShaderVar>& varList);

        GLShader::View glVertexShader;
        GLShader::View glFragmentShader;
        GLProgram::View glShaderProgram;

        ResourceManagerOGL3 resourceManager;

        virtual const StructuredData::ElemLayout& getVertexLayout() const override;
};

}

#endif // PGEINTERNAL_SHADEROGL3_H_INCLUDED

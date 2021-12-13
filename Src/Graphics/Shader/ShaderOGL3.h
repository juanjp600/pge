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

class GraphicsOGL3;
class ShaderOGL3 : public Shader {
    public:
        ShaderOGL3(Graphics& gfx, const FilePath& path);
        ~ShaderOGL3();

        Constant* getVertexShaderConstant(const String& name) override;
        Constant* getFragmentShaderConstant(const String& name) override;

        void useShader();
        void unbindGLAttribs();

    private:
        GraphicsOGL3& graphics;

        class ConstantOGL3 : public Constant {
            public:
                ConstantOGL3(GraphicsOGL3& gfx, GLint glLoc, GLenum glTyp, int glArrSz, std::unique_ptr<byte[]>& data, int offset);

                void setUniform();

            private:
                GraphicsOGL3& graphics;

                GLint glLocation;
                GLenum glType;
                int glArraySize;

                std::unique_ptr<byte[]>& data;
                int offset;

                void setValueInternal(const std::span<byte>& value) override;
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

        std::unique_ptr<byte[]> vertexUniformData;
        std::unique_ptr<byte[]> fragmentUniformData;

        int glSizeToByteSize(GLenum type, int size) const;
        GLenum parsedTypeToGlType(const String& parsedType);
        void decomposeGlType(GLenum compositeType, GLenum& elemType, int& elemCount);

        void extractVertexUniforms(const String& vertexSource);
        void extractVertexAttributes(const String& vertexSource);
        void extractFragmentUniforms(const String& fragmentSource);
        void extractFragmentOutputs(const String& fragmentSource);

        struct ParsedShaderVar {
            String name;
            String type;
        };
        void extractShaderVars(const String& src,const String& varKind,std::vector<ParsedShaderVar>& varList);

        GLShader::View glVertexShader;
        GLShader::View glFragmentShader;
        GLProgram::View glShaderProgram;

        ResourceManagerOGL3 resourceManager;
};

}

#endif // PGEINTERNAL_SHADEROGL3_H_INCLUDED

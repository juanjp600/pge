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
        ShaderOGL3(Graphics* gfx, const FilePath& path);

        Constant* getVertexShaderConstant(const String& name) override;
        Constant* getFragmentShaderConstant(const String& name) override;

        const std::vector<String>& getVertexInputElems() const;

        void useShader();
        void unbindGLAttribs();
    private:
        class ConstantOGL3 : public Constant {
            public:
                ConstantOGL3(GraphicsOGL3* gfx, int loc);

                void setValue(const Matrix4x4f& value) override;
                void setValue(const Vector2f& value) override;
                void setValue(const Vector3f& value) override;
                void setValue(const Vector4f& value) override;
                void setValue(const Color& value) override;
                void setValue(float value) override;
                void setValue(int value) override;

                void setUniform();

            private:
                // TODO: Remove stinky!
                enum class ValueType {
                    MATRIX,
                    VECTOR2F,
                    VECTOR3F,
                    VECTOR4F,
                    COLOR,
                    FLOAT,
                    INT,
                    INVALID
                } valueType = ValueType::INVALID;
                union Value {
                    Matrix4x4f matrixVal = Matrix4x4f();
                    Vector2f vector2fVal;
                    Vector3f vector3fVal;
                    Vector4f vector4fVal;
                    Color colorVal;
                    float floatVal;
                    int intVal;
                    
                    Value();
                } val;

                GraphicsOGL3* graphics;
                int location;
        };

        std::unordered_map<String::Key, ConstantOGL3> vertexShaderConstants;
        std::unordered_map<String::Key, ConstantOGL3> fragmentShaderConstants;
        std::unordered_map<String::Key, ConstantOGL3> samplerConstants;

        struct VertexAttrib {
            String name;
            int location;
            int size;
            GLenum type;
        };

        int stride;
        std::vector<String> vertexInputElems;
        std::vector<VertexAttrib> vertexAttribs;

        struct ShaderVar {
            String type;
            String name;
        };
        void extractShaderVars(const String& src,const String& varKind,std::vector<ShaderVar>& varList);

        GLShader::View glVertexShader;
        GLShader::View glFragmentShader;
        GLProgram::View glShaderProgram;

        ResourceManagerOGL3 resourceManager;
};

}

#endif // PGEINTERNAL_SHADEROGL3_H_INCLUDED

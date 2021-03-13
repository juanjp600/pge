#ifndef PGEINTERNAL_SHADEROGL3_H_INCLUDED
#define PGEINTERNAL_SHADEROGL3_H_INCLUDED

#include <Graphics/Graphics.h>
#include <Shader/Shader.h>
#include <Misc/String.h>
#include <Math/Matrix.h>

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

namespace PGE {

class ShaderOGL3 : public Shader {
    public:
        ShaderOGL3(Graphics* gfx, const FilePath& path);

        Constant* getVertexShaderConstant(String name) override;
        Constant* getFragmentShaderConstant(String name) override;

        const std::vector<String>& getVertexInputElems() const;

        void useShader();
        void unbindGLAttribs();
    private:
        class ConstantOGL3 : public Constant {
            public:
                ConstantOGL3(Graphics* gfx, String nm, int loc);

                void setValue(Matrix4x4f value) override;
                void setValue(Vector2f value) override;
                void setValue(Vector3f value) override;
                void setValue(Vector4f value) override;
                void setValue(Color value) override;
                void setValue(float value) override;
                void setValue(int value) override;

                void setUniform();

                String getName() const;

            private:
                enum class VALUE_TYPE {
                    MATRIX,
                    VECTOR2F,
                    VECTOR3F,
                    VECTOR4F,
                    COLOR,
                    FLOAT,
                    INT
                };
                union Value {
                    Value();
                    Matrix4x4f matrixVal;
                    Vector2f vector2fVal;
                    Vector3f vector3fVal;
                    Vector4f vector4fVal;
                    Color colorVal;
                    float floatVal;
                    int intVal;
                } val;
                VALUE_TYPE valueType;
                Graphics* graphics;
                String name;
                int location;
        };

        std::vector<ConstantOGL3> vertexShaderConstants;
        std::vector<ConstantOGL3> fragmentShaderConstants;
        std::vector<ConstantOGL3> samplerConstants;

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
        void extractShaderVars(const String& src,String varKind,std::vector<ShaderVar>& varList);

        SmartRef<GLuint> glVertexShader;
        SmartRef<GLuint> glFragmentShader;
        SmartRef<GLuint> glShaderProgram;

        SmartOrderedDestructor destructor = 3;

        Graphics* graphics;
};

}

#endif // PGEINTERNAL_SHADEROGL3_H_INCLUDED

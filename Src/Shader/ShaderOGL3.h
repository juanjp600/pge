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
        ShaderOGL3(Graphics* gfx,const String& path);
        virtual ~ShaderOGL3();

        Constant* getVertexShaderConstant(String name);
        Constant* getFragmentShaderConstant(String name);

        const std::vector<String>& getVertexInputElems() const;

        void useShader();
        void unbindGLAttribs();
    private:
        ShaderOGL3(){};

        class ConstantOGL3 : public Constant {
            public:
                ConstantOGL3(Graphics* gfx,String nm, int loc);
                ~ConstantOGL3(){};

                void setValue(Matrix4x4f value);
                void setValue(Vector3f value);
                void setValue(float value);
                void setValue(int value);

                void setUniform();

                String getName() const;
            private:
                enum class VALUE_TYPE {
                    MATRIX,
                    VECTOR3F,
                    FLOAT,
                    INT
                };
                union Value {
                    Value();
                    Matrix4x4f matrixVal;
                    Vector3f vector3fVal;
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

        GLuint glVertexShader;
        GLuint glFragmentShader;
        GLuint glShaderProgram;

        Graphics* graphics;
};

}

#endif

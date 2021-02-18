#ifndef PGE_SHADER_H_INCLUDED
#define PGE_SHADER_H_INCLUDED

#include <Misc/String.h>
#include <Misc/FilePath.h>
#include <Math/Matrix.h>
#include <Math/Vector.h>
#include <Color/Color.h>

namespace PGE {

class Graphics;

class Shader {
    public:
        static Shader* load(Graphics* gfx, const FilePath& path);

        class Constant {
            public:
                virtual void setValue(Matrix4x4f value) = 0;
                virtual void setValue(Vector2f value) = 0;
                virtual void setValue(Vector3f value) = 0;
                virtual void setValue(Vector4f value) = 0;
                virtual void setValue(Color value) = 0;
                virtual void setValue(float value) = 0;
                virtual void setValue(int value) = 0;

            protected:
                Constant(){};
        };
        virtual Constant* getVertexShaderConstant(String constName) = 0;
        virtual Constant* getFragmentShaderConstant(String constName) = 0;

    protected:
        Shader(){};

        FilePath filepath;
};

}

#endif // PGE_SHADER_H_INCLUDED

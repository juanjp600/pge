#ifndef PGE_SHADER_H_INCLUDED
#define PGE_SHADER_H_INCLUDED

#include <String/String.h>
#include <Misc/FilePath.h>
#include <Math/Matrix.h>
#include <Math/Vector.h>
#include <Color/Color.h>

namespace PGE {

class Graphics;

class Shader {
    public:
        static Shader* load(Graphics* gfx, const FilePath& path);
        virtual ~Shader() = default;

        class Constant {
            public:
                virtual void setValue(const Matrix4x4f& value) = 0;
                virtual void setValue(const Vector2f& value) = 0;
                virtual void setValue(const Vector3f& value) = 0;
                virtual void setValue(const Vector4f& value) = 0;
                virtual void setValue(const Color& value) = 0;
                virtual void setValue(float value) = 0;
                virtual void setValue(int value) = 0;

            protected:
                Constant() = default;
                virtual ~Constant() = default;
        };
        virtual Constant* getVertexShaderConstant(const String& constName) = 0;
        virtual Constant* getFragmentShaderConstant(const String& constName) = 0;

    protected:
        Shader() = default;

        FilePath filepath;
};

}

#endif // PGE_SHADER_H_INCLUDED

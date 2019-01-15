#ifndef PGE_SHADER_H_INCLUDED
#define PGE_SHADER_H_INCLUDED

#include <Graphics/Graphics.h>
#include <Misc/String.h>

#include <Math/Matrix.h>
#include <Math/Vector.h>

namespace PGE {

class Shader {
    public:
        static Shader* load(Graphics* gfx, const String& path);
        virtual ~Shader(){};

        class Constant {
            public:
                virtual void setValue(Matrix4x4f value) =0;
                virtual void setValue(Vector3f value) =0;
                virtual void setValue(float value) =0;
                virtual void setValue(int value) =0;
            protected:
                Constant(){};
                ~Constant(){};
        };
        virtual Constant* getVertexShaderConstant(String constName) =0;
        virtual Constant* getFragmentShaderConstant(String constName) =0;
    protected:
        Shader(){};
        String filepath;
};

}

#endif

#include <Shader/Shader.h>

namespace PGE {

class ShaderVK : public Shader {
    public:
        ShaderVK(Graphics* gfx, const FilePath& path);
        ~ShaderVK();

        Constant* getVertexShaderConstant(String name) override;
        Constant* getFragmentShaderConstant(String name) override;

    private:
        Constant* tempConstant; // TODO: Remove with actual implementation.

        void cleanup() override;
        void throwException(String func, String details) override;

        class ConstantVK : public Constant {
            public:
                ConstantVK();
                ~ConstantVK(){};

                void setValue(Matrix4x4f value) override;
                void setValue(Vector2f value) override;
                void setValue(Vector3f value) override;
                void setValue(Vector4f value) override;
                void setValue(Color value) override;
                void setValue(float value) override;
                void setValue(int value) override;

            private:
                void throwException(String func, String details) override;
        };
};

}

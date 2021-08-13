#include <PGE/Graphics/Graphics.h>
#include <PGE/Graphics/Texture.h>
#include <PGE/Graphics/Shader.h>
#include <PGE/Graphics/Mesh.h>
#include <PGE/String/String.h>
#include <PGE/File/FilePath.h>
#include <PGE/Math/Vector.h>
#include <PGE/Math/Matrix.h>
#include <PGE/Color/Color.h>
#include <PGE/Init/Init.h>
#include <PGE/Input/InputManager.h>
#include <PGE/Input/Input.h>
#include <PGE/Math/Math.h>

#include <vector>

using namespace PGE;

class Program {
    private:
        Graphics* graphics;
        Shader* shader;
        Mesh::Material material;
        Mesh* mesh;
        InputManager* inputManager;
        KeyboardInput* escKey;

        StructuredData vertexDataGpuTransform;
        StructuredData vertexDataCpuTransform;
        std::vector<Mesh::Triangle> triangles;

        bool closeRequested;

        Vector4f vertexPositions[5];

        float state1;
        float state2;
        float state3;

    public:
        Program() {
            graphics = Graphics::create("Example 2", 1600, 900, false, Graphics::Renderer::DirectX11);
            graphics->setDepthTest(false);

            shader = Shader::load(*graphics, FilePath::fromStr("Shader2").makeDirectory());
            material = Mesh::Material(*shader, Mesh::Material::Opaque::NO);

            vertexDataGpuTransform = StructuredData(shader->getVertexLayout(), 5);

            vertexPositions[0] = Vector4f(-1.f, -1.f, 0.f, 1.f);
            vertexPositions[1] = Vector4f(-1.f, 1.f, 0.f, 1.f);
            vertexPositions[2] = Vector4f(1.f, 1.f, 0.f, 1.f);
            vertexPositions[3] = Vector4f(1.f, -1.f, 0.f, 1.f);
            vertexPositions[4] = Vector4f(0.f, 0.f, -2.f, 1.f);

            for (int i = 0; i < 5; i++) {
                vertexDataGpuTransform.setValue(i, "position", vertexPositions[i]);
            }

            vertexDataGpuTransform.setValue(0, "color", Colors::RED);
            vertexDataGpuTransform.setValue(1, "color", Colors::RED);
            vertexDataGpuTransform.setValue(2, "color", Colors::RED);
            vertexDataGpuTransform.setValue(3, "color", Colors::RED);
            vertexDataGpuTransform.setValue(4, "color", Colors::RED);

            vertexDataCpuTransform = vertexDataGpuTransform.copy();

            Color blue = Colors::BLUE; blue.alpha = 0.5f;
            vertexDataCpuTransform.setValue(0, "color", blue);
            vertexDataCpuTransform.setValue(1, "color", blue);
            vertexDataCpuTransform.setValue(2, "color", blue);
            vertexDataCpuTransform.setValue(3, "color", blue);
            vertexDataCpuTransform.setValue(4, "color", blue);

            triangles.emplace_back(1, 0, 4);
            triangles.emplace_back(2, 1, 4);
            triangles.emplace_back(3, 2, 4);
            triangles.emplace_back(0, 3, 4);

            mesh = Mesh::create(*graphics);
            mesh->setMaterial(material);
            mesh->setGeometry(vertexDataGpuTransform, triangles);
            
            inputManager = InputManager::create(*graphics);
            escKey = new KeyboardInput(KeyboardInput::Keycode::ESCAPE);
            inputManager->trackInput(escKey);

            state1 = 0.f;
            state2 = 0.f;
            state3 = 0.f;

            closeRequested = false;
        }

        ~Program() {
            inputManager->untrackInput(escKey);
            delete escKey;
            delete inputManager;
            delete mesh;
            delete shader;
            delete graphics;
        }

        bool getCloseRequested() const {
            return closeRequested;
        }

        void update() {
            state1 += 0.1f;
            state2 += 0.05f;
            state3 += 0.01f;

            Matrix4x4f worldMatrix = Matrix4x4f::constructWorldMat(
                Vector3f(0,0,5 + sin(state1)),
                Vector3f(0,0,Math::degToRad(state1)),
                Vector3f(7.f, 3.5f, 1.f));
            Matrix4x4f viewMatrix = Matrix4x4f::constructViewMat(
                Vectors::ZERO3F,
                Vector3f(sin(state2) * sin(state3) * 0,cos(state2) * sin(state3) * 0,1),
                Vector3f(0,1,0));
            Matrix4x4f projectionMatrix = Matrix4x4f::constructPerspectiveMat(
                Math::degToRad(120.f),
                16.f / 9.f,
                4.f,
                6.f);
            Matrix4x4f stackedMatrices = projectionMatrix * viewMatrix * worldMatrix;

            inputManager->update();
            graphics->update();

            graphics->clear(Colors::BLACK);

            shader->getVertexShaderConstant("worldMatrix").setValue(worldMatrix);
            shader->getVertexShaderConstant("viewMatrix").setValue(viewMatrix);
            shader->getVertexShaderConstant("projectionMatrix").setValue(projectionMatrix);
            mesh->setGeometry(vertexDataGpuTransform, triangles);
            mesh->render();

            for (int i = 0; i < 5; i++) {
                vertexDataCpuTransform.setValue(i, "position", stackedMatrices.transform(vertexPositions[i]));
            }

            shader->getVertexShaderConstant("worldMatrix").setValue(Matrices::IDENTITY);
            shader->getVertexShaderConstant("viewMatrix").setValue(Matrices::IDENTITY);
            shader->getVertexShaderConstant("projectionMatrix").setValue(Matrices::IDENTITY);
            mesh->setGeometry(vertexDataCpuTransform, triangles);
            mesh->render();

            graphics->swap();

            closeRequested |= escKey->isHit() || !graphics->isWindowOpen();
        }
};

int PGE::Init::main(const std::vector<String>& args) {
    Program myProgram;
    while (!myProgram.getCloseRequested()) {
        SysEvents::update();
        myProgram.update();
    }
    return 0;
}

#include <PGE/Graphics/Graphics.h>
#include <PGE/Graphics/Material.h>
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
        Material* material;
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
            graphics = Graphics::create("Example 2", 1600, 900, WindowMode::Windowed, Renderer::OpenGL);
            graphics->setDepthTest(false);

            shader = Shader::load(*graphics, FilePath::fromStr("Shader2").makeDirectory());
            material = Material::create(*graphics, *shader, Opaque::NO);

            vertexDataGpuTransform = StructuredData(shader->getVertexLayout(), 5);

            vertexPositions[0] = Vector4f(-1.f, -1.f, 0.f, 1.f);
            vertexPositions[1] = Vector4f(-1.f, 1.f, 0.f, 1.f);
            vertexPositions[2] = Vector4f(1.f, 1.f, 0.f, 1.f);
            vertexPositions[3] = Vector4f(1.f, -1.f, 0.f, 1.f);
            vertexPositions[4] = Vector4f(0.f, 0.f, 0.f, 1.f);

            for (int i = 0; i < 5; i++) {
                vertexDataGpuTransform.setValue(i, "position", vertexPositions[i]);
            }

            vertexDataGpuTransform.setValue(0, "color", Colors::GREEN);
            vertexDataGpuTransform.setValue(1, "color", Colors::RED);
            vertexDataGpuTransform.setValue(2, "color", Colors::RED);
            vertexDataGpuTransform.setValue(3, "color", Colors::GREEN);
            vertexDataGpuTransform.setValue(4, "color", Colors::WHITE);

            vertexDataCpuTransform = vertexDataGpuTransform.copy();

            Color blue = Colors::BLUE; blue.alpha = 0.5f;
            Color white = Colors::WHITE; white.alpha = 0.5f;
            vertexDataCpuTransform.setValue(0, "color", blue);
            vertexDataCpuTransform.setValue(1, "color", blue);
            vertexDataCpuTransform.setValue(2, "color", blue);
            vertexDataCpuTransform.setValue(3, "color", blue);
            vertexDataCpuTransform.setValue(4, "color", white);

            triangles.emplace_back(0, 1, 4);
            triangles.emplace_back(1, 2, 4);
            triangles.emplace_back(2, 3, 4);
            triangles.emplace_back(3, 0, 4);

            triangles.emplace_back(1, 0, 4);
            triangles.emplace_back(2, 1, 4);
            triangles.emplace_back(3, 2, 4);
            triangles.emplace_back(0, 3, 4);

            mesh = Mesh::create(*graphics);
            mesh->setMaterial(material);
            mesh->setGeometry(vertexDataGpuTransform.copy(), triangles);
            
            inputManager = InputManager::create(*graphics);
            escKey = new KeyboardInput(Keycode::ESCAPE);
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
            delete material;
            delete shader;
            delete graphics;
        }

        bool getCloseRequested() const {
            return closeRequested;
        }

        void update() {
            state1 += 0.1f;
            state2 += 0.05f;
            state3 += 0.007f;

            Matrix4x4f worldMatrix = Matrix4x4f::constructWorldMat(
                Vector3f(0,0,0),
                Vector3f(0,0,0),
                Vector3f(7.f, 3.5f, 1.f));
            Vector3f camPos = Vector3f(sin(state2), 0.5f, cos(state2)).normalize() * 5.f + Vector3f(0, 0, 0);
            Vector3f forwardVector = (Vector3f(0, 0, 0) - camPos).normalize();
            Matrix4x4f viewMatrix = Matrix4x4f::constructViewMat(
                camPos,
                forwardVector,
                Vector3f(0,1,0));
            Matrix4x4f projectionMatrix = Matrix4x4f::constructPerspectiveMat(
                Math::degToRad(90.f),
                16.f / 9.f,
                0.01f,
                5.f);
            Matrix4x4f stackedMatrices = projectionMatrix * viewMatrix * worldMatrix;

            inputManager->update();
            graphics->update();

            graphics->clear(Colors::BLACK);

            shader->getVertexShaderConstant("worldMatrix")->setValue(worldMatrix);
            shader->getVertexShaderConstant("viewMatrix")->setValue(viewMatrix);
            shader->getVertexShaderConstant("projectionMatrix")->setValue(projectionMatrix);
            mesh->setGeometry(vertexDataGpuTransform.copy(), triangles);
            mesh->render();

            for (int i = 0; i < 5; i++) {
                vertexDataCpuTransform.setValue(i, "position", stackedMatrices.transform(vertexPositions[i]));
            }

            shader->getVertexShaderConstant("worldMatrix")->setValue(Matrices::IDENTITY);
            shader->getVertexShaderConstant("viewMatrix")->setValue(Matrices::IDENTITY);
            shader->getVertexShaderConstant("projectionMatrix")->setValue(Matrices::IDENTITY);
            mesh->setGeometry(vertexDataCpuTransform.copy(), triangles);
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

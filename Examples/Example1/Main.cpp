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
        
        StructuredData vertexData;
        std::vector<Mesh::Triangle> triangles;

        float state;

        bool closeRequested;

    public:
        Program() {
            graphics = Graphics::create("Example 1", 1600, 900, Graphics::WindowMode::Windowed, Graphics::Renderer::Vulkan);
            shader = Shader::load(*graphics, FilePath::fromStr("Shader1").makeDirectory());
            material = Material::create(*graphics, *shader, Material::Opaque::YES);

            vertexData = StructuredData(shader->getVertexLayout(), 5);
            
            vertexData.setValue(0, "position", Vector4f(-1.f, -1.f, 0.f, 1.f));
            vertexData.setValue(1, "position", Vector4f(-1.f, 1.f, 0.f, 1.f));
            vertexData.setValue(2, "position", Vector4f(1.f, 1.f, 0.f, 1.f));
            vertexData.setValue(3, "position", Vector4f(1.f, -1.f, 0.f, 1.f));
            vertexData.setValue(4, "position", Vector4f(0.f, 0.f, 0.f, 1.f));

            triangles.emplace_back(1, 0, 4);
            triangles.emplace_back(2, 1, 4);
            triangles.emplace_back(3, 2, 4);
            triangles.emplace_back(0, 3, 4);

            mesh = Mesh::create(*graphics);
            mesh->setMaterial(material);
            mesh->setGeometry(vertexData.copy(), triangles);
            
            inputManager = InputManager::create(*graphics);
            escKey = new KeyboardInput(KeyboardInput::Keycode::ESCAPE);
            inputManager->trackInput(escKey);

            state = 0.f;

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
            inputManager->update();
            graphics->update();

            state += 3.f;

            Color color0 = Color::fromHSV(state, 1.f, 1.f, 1.f);
            vertexData.setValue(0, "color", color0);
            Color color1 = Color::fromHSV(state+90.f, 1.f, 1.f, 1.f);
            vertexData.setValue(1, "color", color1);
            Color color2 = Color::fromHSV(state+180.f, 1.f, 1.f, 1.f);
            vertexData.setValue(2, "color", color2);
            Color color3 = Color::fromHSV(state+270.f, 1.f, 1.f, 1.f);
            vertexData.setValue(3, "color", color3);
            Color color4 = Color::lerp(
                Color::lerp(color0, color1, 0.5f),
                Color::lerp(color2, color3, 0.5f),
                0.5f);
            vertexData.setValue(4, "color", color4);

            mesh->setGeometry(vertexData.copy(), triangles);

            graphics->clear(Colors::MAGENTA);
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

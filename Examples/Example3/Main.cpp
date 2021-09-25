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
#include <PGE/Math/Random.h>

#include <vector>
#include <chrono>

using namespace PGE;

class Program {
    private:
        using Clock = std::chrono::steady_clock;

        Graphics* graphics;

        Shader* shader;

        Mesh* mesh;

        Shader* shader2;
        Mesh* mesh2;
        Texture* tex;

        Mesh* mesh3;
        Texture* tex2;

        InputManager* inputManager;
        KeyboardInput* escKey;

        u64 counter = 0;
        Clock::time_point prev = Clock::now();

        bool closeRequested = false;

        Texture* load(const FilePath& path) {
            std::vector<byte> bytes = path.readBytes();
            u32 headerOffset = *(u32*)&bytes[0x000A];
            for (int i = headerOffset; i < bytes.size() - 2; i += 4) {
                std::swap(bytes[i], bytes[i + 2]);
            }
            int dim = sqrt((bytes.size() - headerOffset) / 4);
            return Texture::load(*graphics, dim, dim, bytes.data() + headerOffset, Texture::Format::RGBA32);
        }

    public:
        Program() {
            graphics = Graphics::create("Example 3", 1000, 500, Graphics::WindowMode::Windowed, Graphics::Renderer::Vulkan);
            graphics->setCulling(Graphics::Culling::BACK);

            inputManager = InputManager::create(*graphics);
            escKey = new KeyboardInput(KeyboardInput::Keycode::ESCAPE);
            inputManager->trackInput(escKey);

            mesh = Mesh::create(*graphics);
            shader = Shader::load(*graphics, FilePath::fromStr("Shader3"));
            StructuredData vertices(shader->getVertexLayout(), 4);
            vertices.setValue(0, "position", Vector2f(0, 0));
            vertices.setValue(1, "position", Vector2f(1, 0));
            vertices.setValue(2, "position", Vector2f(0, 1));
            vertices.setValue(3, "position", Vector2f(1, 1));
            vertices.setValue(0, "color", Colors::RED);
            vertices.setValue(1, "color", Colors::GREEN);
            vertices.setValue(2, "color", Colors::GREEN);
            vertices.setValue(3, "color", Colors::BLUE);
            mesh->setMaterial(Mesh::Material(*shader, Mesh::Material::Opaque::NO));
            mesh->setGeometry(std::move(vertices), Mesh::PrimitiveType::TRIANGLE, { 0, 1, 2, 1, 2, 3 });

            shader2 = Shader::load(*graphics, FilePath::fromStr("Shader3.2"));
            
            vertices = StructuredData(shader2->getVertexLayout(), 4);
            vertices.setValue(0, "position", Vector2f(0, 0));
            vertices.setValue(1, "position", Vector2f(-1, 0));
            vertices.setValue(2, "position", Vector2f(0, -1));
            vertices.setValue(3, "position", Vector2f(-1, -1));
            vertices.setValue(0, "uv", Vector2f(0, 1));
            vertices.setValue(1, "uv", Vector2f(1, 1));
            vertices.setValue(2, "uv", Vector2f(0, 0));
            vertices.setValue(3, "uv", Vector2f(1, 0));

            tex = load(FilePath::fromStr("logo.bmp"));
            mesh2 = Mesh::create(*graphics);
            mesh2->setGeometry(vertices.copy(), Mesh::PrimitiveType::TRIANGLE, { 0, 1, 2, 1, 2, 3 });
            mesh2->setMaterial(Mesh::Material(*shader2, *tex, Mesh::Material::Opaque::YES));

            tex2 = load(FilePath::fromStr("juan.bmp"));
            mesh3 = Mesh::create(*graphics);
            mesh3->setGeometry(std::move(vertices), Mesh::PrimitiveType::TRIANGLE, { 2, 1, 0, 3, 2, 1 });
            mesh3->setMaterial(Mesh::Material(*shader2, *tex2, Mesh::Material::Opaque::YES));
        }

        ~Program() {
            inputManager->untrackInput(escKey);
            delete escKey;
            delete inputManager;
            delete mesh3;
            delete tex2;
            delete mesh2;
            delete shader2;
            delete tex;
            delete mesh;
            delete shader;
            delete graphics;
        }

        bool getCloseRequested() const {
            return closeRequested;
        }

        void update() {
            inputManager->update();
            graphics->update();

            Clock::time_point now = Clock::now();
            counter += (now - prev).count();
            prev = now;
            constexpr u64 SECOND = std::chrono::duration_cast<Clock::duration>(std::chrono::seconds(1)).count();
            if (counter > SECOND) {
                counter %= SECOND;
                graphics->setCulling(graphics->getCulling() == Graphics::Culling::BACK ? Graphics::Culling::FRONT : Graphics::Culling::BACK);
            }

            graphics->clear(Colors::GRAY);

            mesh->render();
            mesh2->render();
            mesh3->render();

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

#include <SDL.h>

#include <Init/Init.h>
#include <Graphics/Graphics.h>
#include <Mesh/Mesh.h>
#include <Texture/Texture.h>
#include <Shader/Shader.h>
#include <Material/Material.h>
#include <IO/IO.h>
#include <SysEvents/SysEvents.h>
#include <Math/Matrix.h>
#include <Math/Vector.h>
#include <Color/Color.h>

#include <fstream>
#include <math.h>

using namespace PGE;

int main(int argc, char** argv) {
    InitEnv();

    Graphics* graphics = Graphics::create(1280,720,true);
    IO* io = IO::create(graphics->getWindow());

    Shader* shader = Shader::load(graphics,"default/");

    KeyboardInput testInput = KeyboardInput(SDL_SCANCODE_SPACE);
    io->trackInput(&testInput);
    while (graphics->getWindow()->isOpen()) {
        SysEvents::update();
        io->update();
        graphics->update();

        graphics->clear(Color(0.f,testInput.isDown() ? 1.f : 0.f,1.f,1.f));

        graphics->swap(false);
    }
    io->untrackInput(&testInput);

    delete io;
    delete graphics;

    QuitEnv();

    return 0;
}


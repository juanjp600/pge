#include <SDL.h>
#include <FreeImage.h>

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

#include <math.h>

using namespace PGE;

int main(int argc, char** argv) {
    InitEnv();

    int tick = 0;

    Graphics* graphics = Graphics::create(1280,720,false);
    IO* io = IO::create(graphics->getWindow());

    Texture* texture0 = Texture::load(graphics, "dirtymetal.jpg");
    Texture* texture1 = Texture::load(graphics, "fog.jpg");
    Shader* shader = Shader::load(graphics,"default/");
    std::vector<Texture*> textures; textures.push_back(texture0); textures.push_back(texture1);
    Material* material = new Material(shader,textures);

    Mesh* mesh = Mesh::create(graphics, Primitive::TYPE::TRIANGLE);
    std::vector<Vector2f> uvCoords; uvCoords.push_back(Vector2f(0.f, 0.f)); uvCoords.push_back(Vector2f(0.f, 1.f));
    mesh->addVertex(Vertex(Vector3f(-5.f, -5.f, 0.f), Vector3f(0.f, 0.f, -1.f), uvCoords, Color(1.f, 1.f, 1.f, 0.f)));
    uvCoords.clear(); uvCoords.push_back(Vector2f(0.f, 1.f)); uvCoords.push_back(Vector2f(0.f, 0.f));
    mesh->addVertex(Vertex(Vector3f(-5.f, 5.f, 0.f), Vector3f(0.f, 0.f, -1.f), uvCoords, Color(0.5f, 1.f, 1.f, 1.f)));
    uvCoords.clear(); uvCoords.push_back(Vector2f(1.f, 0.f)); uvCoords.push_back(Vector2f(1.f, 1.f));
    mesh->addVertex(Vertex(Vector3f(5.f, -5.f, 0.f), Vector3f(0.f, 0.f, -1.f), uvCoords, Color(0.75f, 1.f, 1.f, 1.f)));
    uvCoords.clear(); uvCoords.push_back(Vector2f(1.f, 1.f)); uvCoords.push_back(Vector2f(1.f, 0.f));
    mesh->addVertex(Vertex(Vector3f(5.f, 5.f, 0.f), Vector3f(0.f, 0.f, -1.f), uvCoords, Color(1.f, 1.f, 1.f, 1.f)));
    mesh->addPrimitive(Primitive(0, 1, 2));
    mesh->addPrimitive(Primitive(2, 1, 3));
    mesh->addPrimitive(Primitive(1, 0, 2));
    mesh->addPrimitive(Primitive(1, 2, 3));
    mesh->setMaterial(material);
    
    float aspectRatio = static_cast<float>(graphics->viewport.width()) / static_cast<float>(graphics->viewport.height());

    float zoom = 1.f;

    float farPlane = 10.f;
    float nearPlane = 0.01f;

    graphics->projectionMatrix.elements[0][0] = 720.f / 1280.f;
    graphics->projectionMatrix.elements[1][1] = 1.f;
    graphics->projectionMatrix.elements[2][2] = farPlane / (nearPlane - farPlane);
    graphics->projectionMatrix.elements[2][3] = -1.f;
    graphics->projectionMatrix.elements[3][2] = (nearPlane*farPlane / (nearPlane - farPlane));
    graphics->viewMatrix = Matrix4x4f::constructViewMat(Vector3f(0,0,-5),Vector3f(0,0,1),Vector3f(0,-1,0));

    KeyboardInput testInput = KeyboardInput(SDL_SCANCODE_SPACE);
    io->trackInput(&testInput);
    while (graphics->getWindow()->isOpen()) {
        SysEvents::update();
        io->update();
        graphics->update();

        graphics->clear(Color(testInput.isDown()*1.f,0.5f+0.5f*sin(((float)(tick+220))/100.f),0.5f+0.5f*sin(((float)tick)/100.f),1.f));
        
        mesh->worldMatrix = Matrix4x4f::constructWorldMat(Vector3f(0,0,7.f),Vector3f(1.f,1.f,1.f),Vector3f(0.f,((float)tick)/13.f,0.f));
        mesh->render();
        
        graphics->swap(true);

        tick++;
    }
    io->untrackInput(&testInput);
    delete io;
    delete graphics;

    QuitEnv();

    return 0;
}

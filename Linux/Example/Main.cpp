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

#include <math.h>

using namespace PGE;

int main(int argc, char** argv) {
    InitEnv();

    int tick = 0;

    Graphics* graphics = Graphics::create(1280,720,false);
    IO* io = IO::create(graphics->getWindow());

    Texture* texture0 = Texture::load(graphics, "dirtymetal.jpg");
    Texture* texture1 = Texture::load(graphics, "poster.png");
    Texture* texture2 = Texture::create(graphics,256,256,true,nullptr,Texture::FORMAT::RGBA32);//Texture::load(graphics, "poster2.png");
    Texture* texture3 = Texture::create(graphics,256,256,true,nullptr,Texture::FORMAT::RGBA32);
    Shader* shader = Shader::load(graphics,"default/");
    std::vector<Texture*> textures; textures.push_back(texture1);// textures.push_back(texture1);
    Material* material = new Material(shader,textures);

    Mesh* mesh = Mesh::create(graphics, Primitive::TYPE::TRIANGLE);
    std::vector<Vertex> vertices;
    Vertex vert;
    vert.setVector4f("position",Vector4f(-5.f,-5.f,0.f,1.f));
    vert.setVector3f("normal",Vector3f(0.f,0.f,-1.f));
    vert.setVector2f("uv0",Vector2f(0.f,0.f));
    vert.setVector2f("uv1",Vector2f(0.f,1.f));
    vert.setColor("color",Color(1.f,1.f,1.f,1.f));
    vertices.push_back(vert);
    vert.setVector4f("position",Vector4f(-5.f,5.f,0.f,1.f));
    vert.setVector3f("normal",Vector3f(0.f,0.f,-1.f));
    vert.setVector2f("uv0",Vector2f(0.f,1.f));
    vert.setVector2f("uv1",Vector2f(0.f,0.f));
    vert.setColor("color",Color(0.5f,1.f,1.f,1.f));
    vertices.push_back(vert);
    vert.setVector4f("position",Vector4f(5.f,-5.f,0.f,1.f));
    vert.setVector3f("normal",Vector3f(0.f,0.f,-1.f));
    vert.setVector2f("uv0",Vector2f(1.f,0.f));
    vert.setVector2f("uv1",Vector2f(1.f,1.f));
    vert.setColor("color",Color(1.f,1.f,1.f,1.f));
    vertices.push_back(vert);
    vert.setVector4f("position",Vector4f(5.f,5.f,0.f,1.f));
    vert.setVector3f("normal",Vector3f(0.f,0.f,-1.f));
    vert.setVector2f("uv0",Vector2f(1.f,1.f));
    vert.setVector2f("uv1",Vector2f(1.f,0.f));
    vert.setColor("color",Color(1.f,1.f,1.f,1.f));
    vertices.push_back(vert);

    std::vector<Primitive> primitives;
    primitives.push_back(Primitive(0, 1, 2));
    primitives.push_back(Primitive(2, 1, 3));
    primitives.push_back(Primitive(1, 0, 2));
    primitives.push_back(Primitive(1, 2, 3));

    mesh->setGeometry(vertices,primitives);

    mesh->setMaterial(material);

    Mesh* mesh2 = mesh->clone();
    textures.clear(); textures.push_back(texture2);// textures.push_back(texture2);
    Material* material2 = new Material(shader, textures);
    mesh2->setMaterial(material2);

    float aspectRatio = 1280.f / 720.f;

    float zoom = 1.f;

    float farPlane = 10.f;
    float nearPlane = 0.01f;

    Matrix4x4f projectionMatrix = Matrix4x4f::identity;
    projectionMatrix.elements[0][0] = 720.f / 1280.f;
    projectionMatrix.elements[1][1] = 1.f;
    projectionMatrix.elements[2][2] = farPlane / (nearPlane - farPlane);
    projectionMatrix.elements[2][3] = -1.f;
    projectionMatrix.elements[3][2] = (nearPlane*farPlane / (nearPlane - farPlane));
    Matrix4x4f viewMatrix = Matrix4x4f::constructViewMat(Vector3f(0,-8,-3),Vector3f(0,1,1).normalize(),Vector3f(0,-1,0));

    Shader::Constant* projMatrixConstant = shader->getVertexShaderConstant("projectionMatrix");
    projMatrixConstant->setValue(projectionMatrix);
    Shader::Constant* viewMatrixConstant = shader->getVertexShaderConstant("viewMatrix");
    viewMatrixConstant->setValue(viewMatrix);
    Shader::Constant* worldMatrixConstant = shader->getVertexShaderConstant("worldMatrix");

    KeyboardInput testInput = KeyboardInput(SDL_SCANCODE_SPACE);
    io->trackInput(&testInput);

    std::vector<Texture*> renderTargets;
    renderTargets.push_back(texture3); renderTargets.push_back(texture2);

    while (graphics->getWindow()->isOpen()) {
        SysEvents::update();
        io->update();
        graphics->update();

        graphics->setRenderTargets(renderTargets);

        graphics->clear(Color(0.f,0.f,0.f,1.f));
        graphics->setViewport(Rectanglei(0,0,256,256));
        for (int i=0;i<1000;i++) {
            worldMatrixConstant->setValue(Matrix4x4f::constructWorldMat(Vector3f(0, 100-i*2.f, 9.f), Vector3f(1.f, 1.f, 1.f), Vector3f(0.3f, -((float)tick) / 60.f, 0.f)));
            mesh->render();
        }

        graphics->resetRenderTarget();
        graphics->setViewport(Rectanglei(0,0,1280,720));

        graphics->clear(Color(testInput.isDown()*1.f,0.5f+0.5f*sin(((float)(tick+220))/100.f),0.5f+0.5f*sin(((float)tick)/100.f),1.f));

        worldMatrixConstant->setValue(Matrix4x4f::constructWorldMat(Vector3f(0, 0, 8.5f), Vector3f(1.f, 1.f, 1.f), Vector3f(0.f, 0.f, 0.f)));
        mesh2->render();

        worldMatrixConstant->setValue(Matrix4x4f::constructWorldMat(Vector3f(0, 0, 9.f), Vector3f(1.f, 1.f, 1.f), Vector3f(0.3f, -((float)tick) / 60.f, 0.f)));
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

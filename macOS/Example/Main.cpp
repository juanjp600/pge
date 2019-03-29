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
#include <Math/Line.h>
#include <Color/Color.h>
#include <Audio/Audio.h>

#include <fstream>
#include <math.h>

#include <sys/stat.h>

#include "Collision/Collision.h"

using namespace PGE;

static bool fileExists(const char* file) {
    struct stat buf;
    return (stat(file, &buf) == 0);
}

struct RM2 {
    std::vector<Mesh*> meshes;
    std::vector<Texture*> textures;
    std::vector<Material*> materials;
};

RM2 loadRM2(String name,Graphics* graphics,Shader* shader) {
    //NOTE: this is really hacky, DO NOT USE IN SCPCB
    String path = "";
    for (int i=name.size()-1;i>=0;i--) {
        if (name.charAt(i)=='/' || name.charAt(i)=='\\') {
            path = name.substr(0,i+1);
            break;
        }
    }

    std::ifstream file; file.open(name.cstr(),std::ios::binary|std::ios::in);

    RM2 retVal;

    //skip header
    int header;
    file.read((char*)(void*)&header,4);

    //assume textures come first
    char texturePart; file.read(&texturePart,1);
    char textureCount; file.read(&textureCount,1);
    for (int i=0;i<textureCount;i++) {
        char strLen; file.read(&strLen,1);
        String name = "";
        for (int j=0;j<strLen;j++) {
            char chr; file.read(&chr,1);
            name = String(name,chr);
        }

        String texName = "";
        if (name.findFirst("_lm")>-1) {
            texName = path+name+".png";
        } else {
            texName = String("GFX/Map/Textures/"+name+".jpg").resourcePath();
            bool found = fileExists(texName.cstr());
            if (!found) {
                texName = String("GFX/Map/Textures/"+name+".png").resourcePath();
            }

            found = fileExists(texName.cstr());
            if (!found) {
                texName = String("GFX/Map/Textures/dirtymetal.jpg").resourcePath();
            }
        }
        retVal.textures.push_back(Texture::load(graphics, texName));

        char flagSkip; file.read(&flagSkip,1); file.read(&flagSkip,1);
    }

    char partHeader; file.read(&partHeader,1);
    while ((partHeader==2) || (partHeader==3)) {
        Mesh* mesh = Mesh::create(graphics,Primitive::TYPE::TRIANGLE);

        char textureIndex0 = 0; file.read(&textureIndex0,1);
        if (textureIndex0>0) { textureIndex0--; }
        char textureIndex1 = 0; file.read(&textureIndex1,1);
        if (textureIndex1>0) { textureIndex1--; }

        std::vector<Texture*> textures;
        textures.push_back(retVal.textures[textureIndex0]);
        textures.push_back(retVal.textures[textureIndex1]);

        Material* material = new Material(shader,textures);
        retVal.materials.push_back(material);

        mesh->setMaterial(material);

        std::vector<Vertex> vertices;

        unsigned short vertCount = 0; file.read((char*)(void*)&vertCount,2);
        for (int i=0;i<vertCount;i++) {
            Vertex vertex;

            Vector4f pos = Vector4f(0.f,0.f,0.f,1.f);
            Color color = Color(1.f,1.f,1.f,1.f);
            Vector2f uv0 = Vector2f(0.f,0.f);
            Vector2f uv1 = Vector2f(0.f,0.f);

            file.read((char*)(void*)&pos.x,4);
            file.read((char*)(void*)&pos.y,4);
            file.read((char*)(void*)&pos.z,4);

            unsigned char r; unsigned char g; unsigned char b;
            file.read((char*)(void*)&r,1); color.setRedInt(r);
            file.read((char*)(void*)&g,1); color.setGreenInt(g);
            file.read((char*)(void*)&b,1); color.setBlueInt(b);



            file.read((char*)(void*)&uv0.x,4);
            file.read((char*)(void*)&uv0.y,4);

            file.read((char*)(void*)&uv1.x,4);
            file.read((char*)(void*)&uv1.y,4);

            vertex.setVector4f("position",pos);
            vertex.setVector3f("normal",Vector3f::one.normalize());
            vertex.setVector2f("uv0",uv1);
            vertex.setVector2f("uv1",uv0);
            vertex.setColor("color",color);

            vertices.push_back(vertex);
        }

        std::vector<Primitive> tris;

        unsigned short triCount = 0; file.read((char*)(void*)&triCount,2);
        for (int i=0;i<triCount;i++) {
            unsigned short i0 = 0; file.read((char*)(void*)&i0,2);
            unsigned short i1 = 0; file.read((char*)(void*)&i1,2);
            unsigned short i2 = 0; file.read((char*)(void*)&i2,2);
            tris.push_back(Primitive(i0,i1,i2));
        }

        mesh->setGeometry(vertices,tris);

        retVal.meshes.push_back(mesh);

        file.read(&partHeader,1);
    }

    file.close();
    return retVal;
}

#if 1
int PGE::Main() {
    ThreadManager* threadManager = new ThreadManager();
    Audio* audio = new Audio(threadManager);

    Sound* sound = new Sound(audio,String("SFX/Music/The Dread.ogg").resourcePath());
    Sound::Channel* channel = sound->play();
    
    Graphics* graphics = Graphics::create("Mac PGE Test", 1280,720, false);
    IO* io = IO::create(graphics->getWindow());

    Shader* shader = Shader::load(graphics, String("default/").resourcePath());
    Shader* postprocessShader = Shader::load(graphics, String("postprocess/").resourcePath());

    RM2 testRM2 = loadRM2(String("GFX/Map/Rooms/extend_gateb/extend_gateb.rm2").resourcePath(), graphics, shader);

    float retWidth = 1280.f;// * 2;
    float retHeight = 720.f;// * 2;
    float something = 2048.f;// * 2;

    Texture* texture0 = Texture::create(graphics,something,something,true,nullptr,Texture::FORMAT::RGBA32);
    Texture* texture1 = Texture::create(graphics,something,something,true,nullptr,Texture::FORMAT::R32F);

    std::vector<Texture*> renderTargets; renderTargets.push_back(texture0); renderTargets.push_back(texture1);

    std::vector<Texture*> textures; textures.push_back(texture0); textures.push_back(texture1);
    Material* quadMaterial = new Material(postprocessShader,textures);

    Mesh* quad = Mesh::create(graphics, Primitive::TYPE::TRIANGLE);
    std::vector<Vertex> vertices;
    Vertex vert;
    vert.setVector4f("position",Vector4f(-1.f,-1.f,0.f,1.f));
    vert.setVector2f("uv0",Vector2f(0.f,retHeight/something));
    vertices.push_back(vert);
    vert.setVector4f("position",Vector4f(-1.f,1.f,0.f,1.f));
    vert.setVector2f("uv0",Vector2f(0.f,0.f));
    vertices.push_back(vert);
    vert.setVector4f("position",Vector4f(1.f,-1.f,0.f,1.f));
    vert.setVector2f("uv0",Vector2f(retWidth/something,retHeight/something));
    vertices.push_back(vert);
    vert.setVector4f("position",Vector4f(1.f,1.f,0.f,1.f));
    vert.setVector2f("uv0",Vector2f(retWidth/something,0.f));
    vertices.push_back(vert);

    std::vector<Primitive> primitives;
    /*primitives.push_back(Primitive(0, 1, 2));
    primitives.push_back(Primitive(2, 1, 3));*/
    primitives.push_back(Primitive(1, 0, 2));
    primitives.push_back(Primitive(1, 2, 3));

    quad->setGeometry(vertices,primitives);

    quad->setMaterial(quadMaterial);

    float aspectRatio = static_cast<float>(graphics->getViewport().width()) / static_cast<float>(graphics->getViewport().height());

    float zoom = 1.f;

    float farPlane = 10.f;
    float nearPlane = 0.01f;

    Matrix4x4f projectionMatrix = Matrix4x4f::identity;
    projectionMatrix.elements[0][0] = 720.f / 1280.f;
    projectionMatrix.elements[1][1] = 1.f;
    projectionMatrix.elements[2][2] = farPlane / (nearPlane - farPlane);
    projectionMatrix.elements[2][3] = -1.f;
    projectionMatrix.elements[3][2] = (nearPlane*farPlane / (nearPlane - farPlane));
    Matrix4x4f viewMatrix = Matrix4x4f::constructViewMat(Vector3f(0,18,-30),Vector3f(0,-0.6,1).normalize(),Vector3f(0,1,0));

    Shader::Constant* projMatrixConstant = shader->getVertexShaderConstant("projectionMatrix");
    projMatrixConstant->setValue(projectionMatrix);
    Shader::Constant* viewMatrixConstant = shader->getVertexShaderConstant("viewMatrix");
    viewMatrixConstant->setValue(viewMatrix);
    Shader::Constant* worldMatrixConstant = shader->getVertexShaderConstant("worldMatrix");

    KeyboardInput leftInput = KeyboardInput(KeyboardInput::SCANCODE::A);
    io->trackInput(&leftInput);
    KeyboardInput rightInput = KeyboardInput(KeyboardInput::SCANCODE::D);
    io->trackInput(&rightInput);
    KeyboardInput forwardInput = KeyboardInput(KeyboardInput::SCANCODE::W);
    io->trackInput(&forwardInput);
    KeyboardInput backwardInput = KeyboardInput(KeyboardInput::SCANCODE::S);
    io->trackInput(&backwardInput);
    KeyboardInput escInput = KeyboardInput(KeyboardInput::SCANCODE::ESCAPE);
    io->trackInput(&escInput);
    KeyboardInput spaceInput = KeyboardInput(KeyboardInput::SCANCODE::F);
    io->trackInput(&spaceInput);

    float hAngle = 0;
    float vAngle = 0;
    Vector3f cameraPos = Vector3f(0,18,-30);

    io->setMouseVisibility(false);
    io->setMousePosition(Vector2i(640,360));
    bool lockMouse = true;
    while (graphics->getWindow()->isOpen()) {
        SysEvents::update();
        io->update();
        graphics->update();

        if (lockMouse) {
            hAngle -= (float)(io->getMousePosition().x-640)/300.f;
            vAngle -= (float)(io->getMousePosition().y-360)/300.f;
            if (vAngle<-3.14*0.5f) { vAngle = -3.14*0.5f; }
            if (vAngle>3.14*0.5f) { vAngle = 3.14*0.5f; }

            io->setMousePosition(Vector2i(640,360));
        }

        graphics->setRenderTargets(renderTargets);
        graphics->setViewport(Rectanglei(0,0,retWidth,retHeight));
        graphics->clear(Color(0.f,0.f,0.f,1.f));

        worldMatrixConstant->setValue(Matrix4x4f::constructWorldMat(Vector3f(0.f, 0.f, 0.f), Vector3f(0.07f, 0.07f, 0.07f), Vector3f(0.f, 0.f, 0.f)));

        Vector3f lookDir = Vector3f(sin(hAngle)*cos(vAngle),sin(vAngle),cos(hAngle)*cos(vAngle));
        Vector3f upDir = Vector3f(sin(hAngle)*sin(-vAngle),cos(-vAngle),cos(hAngle)*sin(-vAngle));
        Vector3f sideDir = lookDir.crossProduct(upDir);
        viewMatrix = Matrix4x4f::constructViewMat(cameraPos,lookDir,upDir);
        viewMatrixConstant->setValue(viewMatrix);

        if (forwardInput.isDown()) {
            cameraPos = cameraPos.add(lookDir.multiply(0.2f));
        }
        if (backwardInput.isDown()) {
            cameraPos = cameraPos.add(lookDir.multiply(-0.2f));
        }
        if (leftInput.isDown()) {
            cameraPos = cameraPos.add(sideDir.multiply(-0.2f));
        }
        if (rightInput.isDown()) {
            cameraPos = cameraPos.add(sideDir.multiply(0.2f));
        }
        if (escInput.isDown()) {
            break;
        }

        lockMouse = !spaceInput.isDown();
        io->setMouseVisibility(spaceInput.isDown());

        for (int i=0;i<testRM2.meshes.size();i++) {
            testRM2.meshes[i]->render();
        }

        graphics->resetRenderTarget();
        graphics->clear(Color(0.f,0.f,0.f,1.f));
        graphics->setViewport(Rectanglei(0,0,retWidth,retHeight));
        quad->render();

        graphics->swap(false);
    }

    for (int i=0;i<testRM2.meshes.size();i++) {
        delete testRM2.meshes[i];
    }
    testRM2.meshes.clear();

    for (int i=0;i<testRM2.materials.size();i++) {
        delete testRM2.materials[i];
    }
    testRM2.materials.clear();

    for (int i=0;i<testRM2.textures.size();i++) {
        delete testRM2.textures[i];
    }
    testRM2.textures.clear();

    delete quad;
    delete quadMaterial;
    delete texture0; delete texture1;

    delete postprocessShader;
    delete shader;

    delete io;
    delete graphics;
    
    delete threadManager;
    delete channel;
    delete audio;

    return 0;
}
#endif

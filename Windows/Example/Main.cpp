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
#include <Threading/ThreadManager.h>
#include <Audio/Audio.h>

#include <fstream>
#include <math.h>

#include <Windows.h>

#include <iostream>

using namespace PGE;

struct RM2 {
    std::vector<Mesh*>* meshes;
    std::vector<Texture*>* textures;
    std::vector<Texture*>* lmTextures;
    std::vector<Material*>* materials;
};

RM2 loadRM2(String name,Graphics* graphics,Shader* shader,ThreadManager* threadManager) {
    //NOTE: this is really hacky, DO NOT USE IN SCPCB
    String path = "";
    for (int i=name.size()-1;i>=0;i--) {
        if (name.charAt(i)=='/' || name.charAt(i)=='\\') {
            path = name.substr(0,i+1);
            break;
        }
    }
    std::cout<<path.cstr()<<"\n";

    std::ifstream* file = new std::ifstream(); file->open(name.cstr(),std::ios::binary|std::ios::in);

    RM2 retVal;

    retVal.meshes = new std::vector<Mesh*>();
    retVal.textures = new std::vector<Texture*>();
    retVal.materials = new std::vector<Material*>();

    retVal.lmTextures = new std::vector<Texture*>();
    
    //skip header
    int header;
    file->read((char*)(void*)&header,4);

    //assume textures come first
    char texturePart; file->read(&texturePart,1);
    char textureCount; file->read(&textureCount,1);
    for (int i=0;i<textureCount;i++) {
        char strLen; file->read(&strLen,1);
        String name = "";
        for (int j=0;j<strLen;j++) {
            char chr; file->read(&chr,1);
            name = String(name,chr);
        }

        String texName = "";
        if (name.findFirst("_lm")>-1) {
            texName = path+name+"0.png";
            retVal.lmTextures->push_back(Texture::load(graphics,PGE::FileName::create(texName),threadManager));
            texName = path+name+"1.png";
            retVal.lmTextures->push_back(Texture::load(graphics,PGE::FileName::create(texName),threadManager));
            texName = path+name+"2.png";
            retVal.lmTextures->push_back(Texture::load(graphics,PGE::FileName::create(texName),threadManager));
        } else {
            texName = "GFX/Map/Textures/"+name+".jpg";
            WIN32_FIND_DATA FindFileData;
            HANDLE handle = FindFirstFile(texName.cstr(), &FindFileData);
            int found = handle != INVALID_HANDLE_VALUE;
            if (!found) {
                texName = "GFX/Map/Textures/"+name+".png";

                handle = FindFirstFile(texName.cstr(), &FindFileData);
                found = handle != INVALID_HANDLE_VALUE;
                if (!found) {
                    texName = "GFX/Map/Textures/dirtymetal.jpg";
                }
            }
            
            String bumpName = "GFX/Map/Textures/"+name+"_n.jpg";
            handle = FindFirstFile(bumpName.cstr(), &FindFileData);
            found = handle != INVALID_HANDLE_VALUE;
            if (!found) {
                bumpName = "GFX/Map/Textures/blankbump.jpg";
            }

            retVal.textures->push_back(Texture::load(graphics,PGE::FileName::create(bumpName),threadManager));

            retVal.textures->push_back(Texture::load(graphics,PGE::FileName::create(texName),threadManager));
        }
        std::cout<<texName<<"\n";
        
        char flagSkip; file->read(&flagSkip,1); file->read(&flagSkip,1);
    }

    class MainThreadRequest0 : public ThreadManager::MainThreadRequest {
    public:
        Graphics* graphics;
        Mesh* mesh;
        Shader* shader;
        std::vector<Texture*>* textures;
        std::vector<Material*>* materials;
        void execute() {
            mesh = Mesh::create(graphics,Primitive::TYPE::TRIANGLE);

            Material* material = new Material(shader,*textures);
            materials->push_back(material);

            mesh->setMaterial(material);
        }
    } mainThreadRequest0;
    mainThreadRequest0.graphics = graphics;
    mainThreadRequest0.shader = shader;
    mainThreadRequest0.materials = retVal.materials;

    class MainThreadRequest1 : public ThreadManager::MainThreadRequest {
    public:
        std::vector<Mesh*>* meshes;
        Mesh* mesh;
        void execute() {
            meshes->push_back(mesh);
        }
    } mainThreadRequest1;
    mainThreadRequest1.meshes = retVal.meshes;

    class MeshLoadRequest : public ThreadManager::NewThreadRequest {
        public:
            MainThreadRequest0 mainThreadRequest0;
            MainThreadRequest1 mainThreadRequest1;
            std::ifstream* file;
            std::vector<Texture*> loadedTextures;
            std::vector<Texture*> lmTextures;
            void execute() {
                char partHeader; file->read(&partHeader,1);
                while ((partHeader==2) || (partHeader==3)) {
        
                    char textureIndex0 = 0; file->read(&textureIndex0,1);
                    if (textureIndex0>0) { textureIndex0--; }
                    char textureIndex1 = 0; file->read(&textureIndex1,1);
                    if (textureIndex1>0) { textureIndex1--; }

                    std::vector<Texture*> textures;
                    //textures.push_back(loadedTextures[textureIndex0]);
                    textures.push_back(loadedTextures[textureIndex1*2+1]);
                    textures.push_back(lmTextures[0]);
                    textures.push_back(lmTextures[1]);
                    textures.push_back(lmTextures[2]);
                    textures.push_back(loadedTextures[textureIndex1*2]);

                    std::vector<Vertex> vertices;

                    unsigned short vertCount = 0; file->read((char*)(void*)&vertCount,2);
                    for (int i=0;i<vertCount;i++) {
                        Vertex vertex;
            
                        Vector4f pos = Vector4f(0.f,0.f,0.f,1.f);
                        Color color = Color(1.f,1.f,1.f,1.f);
                        Vector2f uv0 = Vector2f(0.f,0.f);
                        Vector2f uv1 = Vector2f(0.f,0.f);

                        file->read((char*)(void*)&pos.x,4);
                        pos.x = -pos.x;
                        file->read((char*)(void*)&pos.y,4);
                        file->read((char*)(void*)&pos.z,4);

                        unsigned char r; unsigned char g; unsigned char b;
                        file->read((char*)(void*)&r,1); color.setRedInt(r);
                        file->read((char*)(void*)&g,1); color.setGreenInt(g);
                        file->read((char*)(void*)&b,1); color.setBlueInt(b);

                        file->read((char*)(void*)&uv0.x,4);
                        file->read((char*)(void*)&uv0.y,4);

                        file->read((char*)(void*)&uv1.x,4);
                        file->read((char*)(void*)&uv1.y,4);

                        vertex.setVector4f("position",pos);
                        vertex.setVector3f("normal",Vector3f::one.normalize());
                        vertex.setVector2f("diffUv",uv0);
                        vertex.setVector2f("lmUv",uv1);
                        vertex.setColor("color",color);

                        vertices.push_back(vertex);
                    }

                    std::vector<Primitive> tris;

                    unsigned short triCount = 0; file->read((char*)(void*)&triCount,2);
                    for (int i=0;i<triCount;i++) {
                        unsigned short i0 = 0; file->read((char*)(void*)&i0,2);
                        unsigned short i1 = 0; file->read((char*)(void*)&i1,2);
                        unsigned short i2 = 0; file->read((char*)(void*)&i2,2);
                        tris.push_back(Primitive(i0,i2,i1));
                    }

                    mainThreadRequest0.textures = &textures;
                    requestExecutionOnMainThread(&mainThreadRequest0);
                    mainThreadRequest0.mesh->setGeometry(vertices,tris);
                    mainThreadRequest0.mesh->updateInternalData();
                    mainThreadRequest1.mesh = mainThreadRequest0.mesh;
                    requestExecutionOnMainThread(&mainThreadRequest1);

                    file->read(&partHeader,1);
                }
                file->close(); delete file;

                markAsDone();
            }
    };
    MeshLoadRequest* meshLoadRequest = new MeshLoadRequest();
    meshLoadRequest->mainThreadRequest0 = mainThreadRequest0;
    meshLoadRequest->mainThreadRequest1 = mainThreadRequest1;
    meshLoadRequest->loadedTextures = *retVal.textures;
    meshLoadRequest->lmTextures = *retVal.lmTextures;
    meshLoadRequest->file = file;
    threadManager->requestExecutionOnNewThread(meshLoadRequest);

    return retVal;
}

int PGEMain::Main() {
    Graphics* graphics = Graphics::create("PGE",1280,720,false);
    IO* io = IO::create(graphics->getWindow());
    ThreadManager* threadManager = new ThreadManager();
    Audio* audio = Audio::create(threadManager);

    Sound* sound = Sound::load(audio, PGE::FileName::create("SFX/Music/The Dread.ogg"));
    Sound::Channel* channel = sound->play();

    Shader* shader = Shader::load(graphics, PGE::FileName::create("default/"));
    Shader* postprocessShader = Shader::load(graphics, PGE::FileName::create("postprocess/"));

    RM2 testRM2 = loadRM2("GFX/Map/Rooms/173new2/173new2.rm2",graphics,shader,threadManager);

    Texture* texture0 = Texture::create(graphics,2048,2048,true,nullptr,Texture::FORMAT::RGBA32);
    Texture* texture1 = Texture::create(graphics,2048,2048,true,nullptr,Texture::FORMAT::R32F);

    std::vector<Texture*> renderTargets; renderTargets.push_back(texture0); renderTargets.push_back(texture1);

    std::vector<Texture*> textures; textures.push_back(texture0); textures.push_back(texture1);
    Material* quadMaterial = new Material(postprocessShader,textures);

    Mesh* quad = Mesh::create(graphics, Primitive::TYPE::TRIANGLE);
    std::vector<Vertex> vertices;
    Vertex vert;
    vert.setVector4f("position",Vector4f(-1.f,-1.f,0.f,1.f));
    vert.setVector2f("uv0",Vector2f(0.f,720.f/2048.f));
    vertices.push_back(vert);
    vert.setVector4f("position",Vector4f(-1.f,1.f,0.f,1.f));
    vert.setVector2f("uv0",Vector2f(0.f,0.f));
    vertices.push_back(vert);
    vert.setVector4f("position",Vector4f(1.f,-1.f,0.f,1.f));
    vert.setVector2f("uv0",Vector2f(1280.f/2048.f,720.f/2048.f));
    vertices.push_back(vert);
    vert.setVector4f("position",Vector4f(1.f,1.f,0.f,1.f));
    vert.setVector2f("uv0",Vector2f(1280.f/2048.f,0.f));
    vertices.push_back(vert);

    std::vector<Primitive> primitives;
    primitives.push_back(Primitive(0, 1, 2));
    primitives.push_back(Primitive(2, 1, 3));
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

    KeyboardInput testInput = KeyboardInput(KeyboardInput::SCANCODE::SPACE);
    io->trackInput(&testInput);
    KeyboardInput leftInput = KeyboardInput(KeyboardInput::SCANCODE::A);
    io->trackInput(&leftInput);
    KeyboardInput rightInput = KeyboardInput(KeyboardInput::SCANCODE::D);
    io->trackInput(&rightInput);
    KeyboardInput forwardInput = KeyboardInput(KeyboardInput::SCANCODE::W);
    io->trackInput(&forwardInput);
    KeyboardInput backwardInput = KeyboardInput(KeyboardInput::SCANCODE::S);
    io->trackInput(&backwardInput);

    float hAngle = 0;
    float vAngle = 0;
    Vector3f cameraPos = Vector3f(0,18,-30);

    io->setMouseVisibility(false);
    io->setMousePosition(Vector2i(640,360));
    while (graphics->getWindow()->isOpen()) {
        SysEvents::update();
        io->update();
        graphics->update();
        threadManager->update();

        hAngle -= (float)(io->getMousePosition().x-640)/300.f;
        vAngle -= (float)(io->getMousePosition().y-360)/300.f;
        if (vAngle<-3.14*0.5f) { vAngle = -3.14*0.5f; }
        if (vAngle>3.14*0.5f) { vAngle = 3.14*0.5f; }
        io->setMousePosition(Vector2i(640,360));

        graphics->setRenderTargets(renderTargets);
        graphics->setViewport(Rectanglei(0,0,1280,720));
        graphics->clear(Color(1.f,0.f,1.f,0.1f));

        worldMatrixConstant->setValue(Matrix4x4f::constructWorldMat(Vector3f(0.f, 0.f, 0.f), Vector3f(0.07f, 0.07f, 0.07f), Vector3f(0.f, 0.f, 0.f)));

        Vector3f lookDir = Vector3f(sin(hAngle)*cos(vAngle),sin(vAngle),cos(hAngle)*cos(vAngle));
        Vector3f upDir = Vector3f(sin(hAngle)*sin(-vAngle),cos(-vAngle),cos(hAngle)*sin(-vAngle));
        Vector3f sideDir = lookDir.crossProduct(upDir);
        viewMatrix = Matrix4x4f::constructViewMat(cameraPos,lookDir,upDir);
        viewMatrixConstant->setValue(viewMatrix);

        if (forwardInput.isDown()) {
            cameraPos = cameraPos.add(lookDir.multiply(0.25f));
        }
        if (backwardInput.isDown()) {
            cameraPos = cameraPos.add(lookDir.multiply(-0.25f));
        }
        if (leftInput.isDown()) {
            cameraPos = cameraPos.add(sideDir.multiply(-0.25f));
        }
        if (rightInput.isDown()) {
            cameraPos = cameraPos.add(sideDir.multiply(0.25f));
        }

        for (int i=0;i<testRM2.meshes->size();i++) {
            (*testRM2.meshes)[i]->render();
        }

        graphics->resetRenderTarget();
        graphics->clear(Color(0.f,0.f,0.f,1.f));
        graphics->setViewport(Rectanglei(0,0,1280,720));
        quad->render();
        
        graphics->swap(false);
    }
    io->untrackInput(&testInput);

    delete threadManager;

    for (int i=0;i<testRM2.meshes->size();i++) {
        delete (*testRM2.meshes)[i];
    }
    delete testRM2.meshes;

    for (int i=0;i<testRM2.materials->size();i++) {
        delete (*testRM2.materials)[i];
    }
    delete testRM2.materials;
    
    for (int i=0;i<testRM2.textures->size();i++) {
        delete (*testRM2.textures)[i];
    }
    delete testRM2.textures;

    for (int i=0;i<testRM2.lmTextures->size();i++) {
        delete (*testRM2.lmTextures)[i];
    }
    delete testRM2.lmTextures;

    delete quad;
    delete quadMaterial;
    delete texture0; delete texture1;

    delete postprocessShader;
    delete shader;

    delete channel;

    delete audio;
    delete io;
    delete graphics;

    return 0;
}

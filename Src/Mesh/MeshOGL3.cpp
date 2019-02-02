#include <Mesh/Mesh.h>
#include "MeshOGL3.h"
#include <Window/Window.h>
#include "../Window/WindowOGL3.h"
#include <Graphics/Graphics.h>
#include "../Graphics/GraphicsOGL3.h"
#include <Shader/Shader.h>
#include "../Shader/ShaderOGL3.h"
#include <Texture/Texture.h>
#include "../Texture/TextureOGL3.h"

using namespace PGE;

Mesh* Mesh::create(Graphics* gfx, Primitive::TYPE pt) {
    return new MeshOGL3(gfx, pt);
}

MeshOGL3::MeshOGL3(Graphics* gfx,Primitive::TYPE pt) {
    graphics = gfx; ((GraphicsOGL3*)graphics)->takeGlContext();

    primitiveType = pt;

    glVertexData.clear(); glIndexData.clear();

    glGenBuffers(1, &glVertexBufferObject);
    glGenBuffers(1, &glIndexBufferObject);

    glGenVertexArrays(1, &glVertexArrayObject);
}

MeshOGL3::~MeshOGL3() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    glDeleteBuffers(1, &glVertexBufferObject);
    glDeleteBuffers(1, &glIndexBufferObject);

    glDeleteVertexArrays(1,&glVertexArrayObject);
}

void MeshOGL3::updateInternalData() {
    glVertexData.clear(); glIndexData.clear();

    for (int i=0;i<vertices.size();i++) {
        //TODO: implement
    }

    for (int i=0;i<primitives.size();i++) {
        glIndexData.push_back(primitives[i].a);
        glIndexData.push_back(primitives[i].b);
        if (primitiveType==Primitive::TYPE::TRIANGLE) {
            glIndexData.push_back(primitives[i].c);
        }
    }

    //TODO: determine when we should use GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, glVertexData.size()*sizeof(GLfloat),glVertexData.data(),GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,glIndexData.size()*sizeof(GLuint),glIndexData.data(),GL_STATIC_DRAW);
}

void MeshOGL3::render() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    glBindVertexArray(glVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER,glVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glIndexBufferObject);

    if (isDirty) {
        updateInternalData(); isDirty = false;
    }

    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D,material->getTexture()->glTexture); //TODO: fix

    //glUseProgram(material->getShader()->glShaderProgram); //TODO: fix

    //material->getShader()->bindGLAttribs(worldMatrix); //TODO: fix

    GLenum glPrimitiveType = GL_TRIANGLES;
    int glIndexMultiplier = 3;
    if (primitiveType==Primitive::TYPE::LINE) {
        glPrimitiveType=GL_LINES;
        glIndexMultiplier = 2;
    }

    glDrawElements(glPrimitiveType,primitives.size()*glIndexMultiplier,GL_UNSIGNED_INT,nullptr);

    glBindVertexArray(0);
}


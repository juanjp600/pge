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

    material = nullptr;

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
    if (!mustUpdateInternalData) { return; }

    glVertexData.clear(); glIndexData.clear();
    
    const std::vector<String>& vertexInputElems = ((ShaderOGL3*)material->getShader())->getVertexInputElems();
    int* indexHints = new int[vertexInputElems.size()];
    for (int j=0;j<vertexInputElems.size();j++) {
        indexHints[j] = 0;
    }
    for (int i=0;i<vertices.size();i++) {
        for (int j=0;j<vertexInputElems.size();j++) {
            const Vertex::Property& prop = vertices[i].getProperty(vertexInputElems[j],indexHints[j]);
            indexHints[j] = prop.index;
            switch (prop.type) {
                case Vertex::PROPERTY_TYPE::FLOAT: {
                    int offset = glVertexData.size();
                    glVertexData.resize(offset+sizeof(float));
                    memcpy(&(glVertexData[offset]),&(prop.value.floatVal),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::UINT: {
                    int offset = glVertexData.size();
                    glVertexData.resize(offset+sizeof(uint32_t));
                    uint32_t uint = prop.value.uintVal;
                    memcpy(&(glVertexData[offset]),&uint,sizeof(uint32_t));
                } break;
                case Vertex::PROPERTY_TYPE::VECTOR2F: {
                    int offset = glVertexData.size();
                    glVertexData.resize(offset+(sizeof(float)*2));
                    memcpy(&(glVertexData[offset]),&(prop.value.vector2fVal.x),sizeof(float));
                    memcpy(&(glVertexData[offset])+sizeof(float),&(prop.value.vector2fVal.y),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::VECTOR3F: {
                    int offset = glVertexData.size();
                    glVertexData.resize(offset+(sizeof(float)*3));
                    memcpy(&(glVertexData[offset]),&(prop.value.vector3fVal.x),sizeof(float));
                    memcpy(&(glVertexData[offset])+sizeof(float),&(prop.value.vector3fVal.y),sizeof(float));
                    memcpy(&(glVertexData[offset])+(sizeof(float)*2),&(prop.value.vector3fVal.z),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::VECTOR4F: {
                    int offset = glVertexData.size();
                    glVertexData.resize(offset+(sizeof(float)*4));
                    memcpy(&(glVertexData[offset]),&(prop.value.vector4fVal.x),sizeof(float));
                    memcpy(&(glVertexData[offset])+sizeof(float),&(prop.value.vector4fVal.y),sizeof(float));
                    memcpy(&(glVertexData[offset])+(sizeof(float)*2),&(prop.value.vector4fVal.z),sizeof(float));
                    memcpy(&(glVertexData[offset])+(sizeof(float)*3),&(prop.value.vector4fVal.w),sizeof(float));
                } break;
                case Vertex::PROPERTY_TYPE::COLOR: {
                    int offset = glVertexData.size();
                    glVertexData.resize(offset+(sizeof(float)*4));
                    memcpy(&(glVertexData[offset]),&(prop.value.colorVal.red),sizeof(float));
                    memcpy(&(glVertexData[offset])+sizeof(float),&(prop.value.colorVal.green),sizeof(float));
                    memcpy(&(glVertexData[offset])+(sizeof(float)*2),&(prop.value.colorVal.blue),sizeof(float));
                    memcpy(&(glVertexData[offset])+(sizeof(float)*3),&(prop.value.colorVal.alpha),sizeof(float));
                } break;
            }
        }
    }
    delete[] indexHints;

    for (int i=0;i<primitives.size();i++) {
        glIndexData.push_back(primitives[i].a);
        glIndexData.push_back(primitives[i].b);
        if (primitiveType==Primitive::TYPE::TRIANGLE) {
            glIndexData.push_back(primitives[i].c);
        }
    }
}

void MeshOGL3::uploadInternalData() {
    if (mustUpdateInternalData) { updateInternalData(); }
    if (!mustReuploadInternalData) { return; }

    //TODO: determine when we should use GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, glVertexData.size(),glVertexData.data(),GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,glIndexData.size()*sizeof(GLuint),glIndexData.data(),GL_STATIC_DRAW);
}

void MeshOGL3::render() {
    ((GraphicsOGL3*)graphics)->takeGlContext();

    glBindVertexArray(glVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER,glVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glIndexBufferObject);

    updateInternalData();
    uploadInternalData();

    GLenum glTextureLayers[] = {
        GL_TEXTURE0,
        GL_TEXTURE1,
        GL_TEXTURE2,
        GL_TEXTURE3,
        GL_TEXTURE4,
        GL_TEXTURE5,
        GL_TEXTURE6,
        GL_TEXTURE7
    };

    for (int i=0;i<material->getTextureCount();i++) {
        glActiveTexture(glTextureLayers[i]);
        glBindTexture(GL_TEXTURE_2D,((TextureOGL3*)material->getTexture(i))->getGlTexture());
    }

    ((ShaderOGL3*)material->getShader())->useShader();

    GLenum glPrimitiveType = GL_TRIANGLES;
    int glIndexMultiplier = 3;
    if (primitiveType==Primitive::TYPE::LINE) {
        glPrimitiveType=GL_LINES;
        glIndexMultiplier = 2;
    }

    glDepthMask(isOpaque());
    glColorMask(true,true,true,!isOpaque());

    glDrawElements(glPrimitiveType,primitives.size()*glIndexMultiplier,GL_UNSIGNED_INT,nullptr);

    ((ShaderOGL3*)material->getShader())->unbindGLAttribs();
    glBindVertexArray(0);
}


#include "../GraphicsOGL3.h"

using namespace PGE;

MeshOGL3::MeshOGL3(Graphics& gfx) : resourceManager(gfx), GraphicsReferencer(gfx) {
    graphics.takeGlContext();

    glVertexBufferObject = resourceManager.addNewResource<GLBuffer>();
    glIndexBufferObject = resourceManager.addNewResource<GLBuffer>();

    glVertexArrayObject = resourceManager.addNewResource<GLVertexArray>();
}

void MeshOGL3::uploadInternalData() {
    if (!mustReuploadInternalData) { return; }

    GLuint glError = GL_NO_ERROR;

    //TODO: determine when we should use GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, vertices.getDataSize(), vertices.getData(),GL_STATIC_DRAW);
    glError = glGetError();
    PGE_ASSERT(glError == GL_NO_ERROR, "Failed to create data store for vertex buffer (GLERROR: " + String::format(glError, "%u") + ")");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLuint),indices.data(),GL_STATIC_DRAW);
    glError = glGetError();
    PGE_ASSERT(glError == GL_NO_ERROR, "Failed to create data store for index buffer (GLERROR: " + String::format(glError, "%u") + ")");

    mustReuploadInternalData = false;
}

const static GLenum glTextureLayers[] = {
    GL_TEXTURE0,
    GL_TEXTURE1,
    GL_TEXTURE2,
    GL_TEXTURE3,
    GL_TEXTURE4,
    GL_TEXTURE5,
    GL_TEXTURE6,
    GL_TEXTURE7
};

void MeshOGL3::render() {
    if (!primitiveType.has_value()) { return; }

    graphics.takeGlContext();

    glBindVertexArray(glVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER,glVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glIndexBufferObject);

    uploadInternalData();

    for (int i=0;i<material->getTextureCount();i++) {
        glActiveTexture(glTextureLayers[i]);
        glBindTexture(GL_TEXTURE_2D,((TextureOGL3&)material->getTexture(i)).getGlTexture());
    }

    ((ShaderOGL3&)material->getShader()).useShader();

    GLenum glPrimitiveType = GL_TRIANGLES;
    if (primitiveType==PrimitiveType::LINE) {
        glPrimitiveType=GL_LINES;
    }

    glDepthMask(isOpaque());
    glColorMask(true,true,true,!isOpaque());

    glDrawElements(glPrimitiveType,(GLsizei)indices.size(),GL_UNSIGNED_INT,nullptr);

    ((ShaderOGL3&)material->getShader()).unbindGLAttribs();
    glBindVertexArray(0);
}


#ifndef PGEINTERNAL_MESHOGL3_H_INCLUDED
#define PGEINTERNAL_MESHOGL3_H_INCLUDED

#include <Mesh/Mesh.h>

#include <vector>

#include <glad/gl.h>

#include "../ResourceManagement/OGL3.h"
#include "../ResourceManagement/ResourceManagerOGL3.h"

namespace PGE {

class MeshOGL3 : public Mesh {
    public:
        MeshOGL3(Graphics* gfx, Primitive::Type pt);

        void updateInternalData() override;

        void render() override;

    private:
        void uploadInternalData() override;

        GLBuffer::Ref glVertexBufferObject;
        GLBuffer::Ref glIndexBufferObject;

        GLVertexArray::Ref glVertexArrayObject;

        ResourceManagerOGL3 resourceManager;

        std::vector<byte> glVertexData;
        std::vector<GLuint> glIndexData;
};

}

#endif // PGEINTERNAL_MESHOGL3_H_INCLUDED

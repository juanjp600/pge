#ifndef PGEINTERNAL_MESHOGL3_H_INCLUDED
#define PGEINTERNAL_MESHOGL3_H_INCLUDED

#include <Graphics/Graphics.h>
#include <Mesh/Mesh.h>

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

#include <vector>

namespace PGE {

struct MeshOGL3 : public Mesh {
    public:
        MeshOGL3(Graphics* gfx,Primitive::TYPE pt);
        virtual ~MeshOGL3();

        virtual void render();
    private:
        virtual void updateInternalData();

        GLuint glVertexBufferObject;
        GLuint glIndexBufferObject;

        GLuint glVertexArrayObject;

        std::vector<uint8_t> glVertexData;
        std::vector<GLuint> glIndexData;
};

}

#endif

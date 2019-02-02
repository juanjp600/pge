#ifndef PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED
#define PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED

#include <Graphics/Graphics.h>
#include <Texture/Texture.h>

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

namespace PGE {

class GraphicsOGL3 : public Graphics {
    public:
        GraphicsOGL3(int w=1280,int h=720,bool fs=false);
        ~GraphicsOGL3();

        virtual void update();

        virtual void clear(Color color);

        virtual void setRenderTarget(Texture* renderTarget){}
        virtual void setRenderTargets(std::vector<Texture*> renderTargets){}
        virtual void resetRenderTarget(){}

        virtual void setViewport(Rectanglei vp);

        void takeGlContext();
    private:
        Rectanglei currentViewport;
};

}

#endif

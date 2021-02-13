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
        GraphicsOGL3(String name,int w=1280,int h=720,bool fs=false);
        ~GraphicsOGL3();

        virtual Renderer getRenderer() override;

        virtual void update() override;

        virtual void clear(Color color) override;
    
        virtual void setDepthTest(bool enabled) override;
        virtual bool getDepthTest() const override;

        virtual void setRenderTarget(Texture* renderTarget) override;
        virtual void setRenderTargets(std::vector<Texture*> renderTargets) override;
        virtual void resetRenderTarget() override;

        virtual void setViewport(Rectanglei vp) override;

        void takeGlContext();
    
    private:
        GLuint glFramebuffer;

        Rectanglei currentViewport;
    
        bool depthTestEnabled;

        void throwException(String func,String details) override;
        void cleanup() override;
};

}

#endif

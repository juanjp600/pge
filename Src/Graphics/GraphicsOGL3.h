#ifndef PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED
#define PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED

#include <Graphics/Graphics.h>
#include "GraphicsInternal.h"

#include "../Misc/SmartPrimitive.h"

#include <SDL.h>
#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#endif
#include <OpenGL/GL.h>
#endif

namespace PGE {

class Texture;

class GraphicsOGL3 : public GraphicsInternal {
    public:
        GraphicsOGL3(String name,int w,int h,bool fs);

        virtual void update() override;
        virtual void swap() override;

        virtual void clear(Color color) override;
    
        virtual void setDepthTest(bool enabled) override;

        virtual void setRenderTarget(Texture* renderTarget) override;
        virtual void setRenderTargets(std::vector<Texture*> renderTargets) override;
        virtual void resetRenderTarget() override;

        virtual void setViewport(Rectanglei vp) override;

        virtual void setVsync(bool isEnabled) override;

        void takeGlContext();
        SDL_GLContext getGlContext() const;

    private:
        SmartPrimitive<SDL_GLContext> glContext;

        SmartPrimitive<GLuint> glFramebuffer;
};

}

#endif // PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED

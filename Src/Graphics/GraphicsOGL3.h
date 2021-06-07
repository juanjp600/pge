#ifndef PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED
#define PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED

#include "GraphicsInternal.h"

#include "../ResourceManagement/OGL3.h"
#include "../ResourceManagement/ResourceManagerOGL3.h"

#include <SDL.h>

namespace PGE {

class Texture;

class GraphicsOGL3 : public GraphicsInternal {
    public:
        GraphicsOGL3(const String& name,int w,int h,bool fs);

        void update() override;
        void swap() override;

        void clear(Color color) override;

        void setRenderTarget(Texture* renderTarget) override;
        void setRenderTargets(const std::vector<Texture*>& renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(const Rectanglei& vp) override;

        void setDepthTest(bool isEnabled) override;
        void setVsync(bool isEnabled) override;
        void setCulling(Culling mode) override;

        void takeGlContext();
        SDL_GLContext getGlContext() const;

        PGE_GFX_OBJ_DEC

    private:
        GLContext::View glContext;
        GLFramebuffer::View glFramebuffer;

        ResourceManagerOGL3 resourceManager;
};

}

#endif // PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED

#ifndef PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED
#define PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED

#include "GraphicsInternal.h"

#include "Shader/ShaderOGL3.h"
#include "Mesh/MeshOGL3.h"
#include "Texture/TextureOGL3.h"

#include "../ResourceManagement/OGL3.h"
#include "../ResourceManagement/ResourceManagerOGL3.h"

#include <SDL.h>

namespace PGE {

class GraphicsOGL3 : public GraphicsSpecialized<ShaderOGL3, MeshOGL3, TextureOGL3> {
    public:
        GraphicsOGL3(const String& name, int w, int h, int x, int y, bool fs);

        void update() override;
        void swap() override;

        void clear(const Color& color) override;

        void setRenderTarget(Texture& renderTarget) override;
        void setRenderTargets(const ReferenceVector<Texture>& renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(const Rectanglei& vp) override;

        void setDepthTest(bool isEnabled) override;
        void setVsync(bool isEnabled) override;
        void setCulling(Culling mode) override;

        void takeGlContext();
        SDL_GLContext getGlContext() const;

    private:
        GLContext::View glContext;
        GLFramebuffer::View glFramebuffer;

        ResourceManagerOGL3 resourceManager;
};

}

#endif // PGEINTERNAL_GRAPHICSOGL3_H_INCLUDED

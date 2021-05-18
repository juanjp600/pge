#ifndef PGE_GRAPHICS_H_INCLUDED
#define PGE_GRAPHICS_H_INCLUDED

#include <vector>
#include <list>

#include "../../Src/ResourceManagement/Misc.h"

#include <Math/Rectangle.h>
#include <Color/Color.h>
#include <SysEvents/SysEvents.h>

struct SDL_Window;

namespace PGE {

class Texture;

class Graphics {
    public:
        enum class Renderer {
            OpenGL,
            DirectX11,
            Vulkan,
            Default
        };

        static Graphics* create(const String& name="PGE Application", int w=1280, int h=720, bool fs=false, Renderer r=Renderer::Default);

        static const std::list<Graphics*>& getActiveInstances();

        Graphics(const String& name, int w, int h, bool fs, uint32_t windowFlags);
        virtual ~Graphics();
        
        virtual void update();
        virtual void swap() = 0;

        virtual void clear(Color color) = 0;

        virtual void setRenderTarget(Texture* renderTarget) = 0;
        virtual void setRenderTargets(const std::vector<Texture*>& renderTargets) = 0;
        virtual void resetRenderTarget() = 0;

        virtual void setViewport(const Rectanglei& vp) = 0;
        const Rectanglei& getViewport() const;

        int getWidth() const;
        int getHeight() const;

        bool isWindowOpen() const;
        bool isWindowFocused() const;

        virtual void setDepthTest(bool isEnabled);
        virtual bool getDepthTest() const;

        virtual void setVsync(bool isEnabled);
        virtual bool getVsync() const;

        String getInfo() const;
        virtual String getRendererName() const = 0;

    protected:
        String caption;

        Rectanglei viewport;

        int width; int height; bool fullscreen;

        bool open;
        bool focused;

        bool depthTest;
        bool vsync;

        // Base class always automatically takes care of SysEvents and the window.
        WindowEventSubscriber::View eventSubscriber;
        SDLWindow::View sdlWindow;

    private:
        ResourceManager resourceManager;
};

}

#endif // PGE_GRAPHICS_H_INCLUDED

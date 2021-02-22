#ifndef PGE_GRAPHICS_H_INCLUDED
#define PGE_GRAPHICS_H_INCLUDED

#include <vector>

#include <Math/Rectangle.h>
#include <Color/Color.h>
#include <SysEvents/SysEvents.h>

#include "../../Src/Misc/SmartPrimitive.h"

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

        static Graphics* create(String name="PGE Application", int w=1280, int h=720, bool fs=false, Renderer r=Renderer::Default);

        Graphics(String name, int w, int h, bool fs);
        virtual ~Graphics() {};
        
        virtual void update();
        virtual void swap() = 0;

        virtual void clear(Color color) = 0;

        virtual void setRenderTarget(Texture* renderTarget) = 0;
        virtual void setRenderTargets(std::vector<Texture*> renderTargets) = 0;
        virtual void resetRenderTarget() = 0;

        virtual void setViewport(Rectanglei vp) = 0;
        Rectanglei getViewport() const;

        int getWidth() const;
        int getHeight() const;

        bool isWindowOpen() const;
        bool isWindowFocused() const;

        virtual void setDepthTest(bool isEnabled);
        virtual bool getDepthTest() const;

        virtual void setVsync(bool isEnabled);
        virtual bool getVsync() const;

    protected:
        Renderer renderer;

        String caption;

        Rectanglei viewport;

        int width; int height; bool fullscreen;

        bool open;
        bool focused;

        bool depthTest;
        bool vsync;

        // Base class always automatically takes care of SysEvents.
        SmartRef<SysEvents::Subscriber*> eventSubscriber;

        SmartRef<SDL_Window*> sdlWindow;

    private:
        SmartOrderedDestructor destructor = 2;
};

}

#endif // PGE_GRAPHICS_H_INCLUDED

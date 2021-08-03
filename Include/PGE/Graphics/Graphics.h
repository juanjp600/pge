#ifndef PGE_GRAPHICS_H_INCLUDED
#define PGE_GRAPHICS_H_INCLUDED

#include <vector>

#include <PGE/ResourceManagement/ResourceManager.h>
#include <PGE/SysEvents/SysEvents.h>
#include <PGE/Math/Rectangle.h>
#include <PGE/Color/Color.h>

namespace PGE {

class Texture;

/// Main class for managing everything graphics related.
/// By default z-buffering, v-sync and backface culling are enabled.
class Graphics {
    public:
        /// The type of renderer to use.
        enum class Renderer {
            OpenGL,
            DirectX11,
            /// Will choose the optimal renderer, depending on the platform.
            /// Windows: DX11
            /// 
            /// Everything else: OGL
            Default,
        };

        /// Factory method.
        static Graphics* create(const String& name = "PGE Application", int w = 1280, int h = 720, bool fs = false, Renderer r = Renderer::Default);

        /// An instance of the Graphics class is considered active from when it was created via #create, until it is deleted.

        virtual ~Graphics();
        
        /// Updates native window functionality.
        /// E.g. minimyzing, closing.
        /// 
        /// **Requires** a call to #PGE::SysEvents::update to function at all.
        virtual void update();

        /// Presents previously rendered image to the screen.
        virtual void swap() = 0;

        /// Clears the screen.
        /// In order to actually display the changes, call #swap.
        /// Independent of #setViewport.
        virtual void clear(const Color& color) = 0;

        /// Sets the current render target to the specified texture.
        /// Texture must support being rendered to.
        virtual void setRenderTarget(Texture* renderTarget) = 0;

        /// Sets the current render target to the specified textures.
        /// All textures must support being rendered to.
        virtual void setRenderTargets(const std::vector<Texture*>& renderTargets) = 0;

        /// Resets the render target to a buffer, which can be presented to the screen using #swap.
        virtual void resetRenderTarget() = 0;

        /// Sets the size and position of the viewport into which rendering operatons take place.
        /// In pixels with the windows's top left corner as (0, 0).
        /// 
        /// Useful for rendering multiple views at once.
        virtual void setViewport(const Rectanglei& vp) = 0;
        const Rectanglei& getViewport() const;

        /// Gets the window's dimensions in pixels.
        const Vector2i& getDimensions() const; float getAspectRatio() const;

        bool isWindowOpen() const;
        bool isWindowFocused() const;

        /// Whether z-buffering is enabled.
        /// Default is enabled.
        virtual void setDepthTest(bool isEnabled);
        virtual bool getDepthTest() const;

        /// Default is enabled.
        virtual void setVsync(bool isEnabled);
        virtual bool getVsync() const;

        /// Default is back culling.
        enum class Culling {
            BACK,
            FRONT,
            NONE,
        };
        virtual void setCulling(Culling mode);
        virtual Culling getCulling() const;

        /// Gets implementation defined debug information about a graphics object.
        virtual String getInfo() const = 0;

    protected:
        String caption;

        Rectanglei viewport;

        Vector2i dimensions; float aspectRatio;
        bool fullscreen;

        bool open;
        bool focused;

        bool depthTest;
        bool vsync;
        Culling cullingMode;

        Graphics(const String& name, int w, int h, bool fs);
    protected:
        ResourceManager resourceManager;
    private:
        // Base class always automatically takes care of SysEvents and the window.
        class WindowEventSubscriber : public Resource<SysEvents::Subscriber*> {
            public:
                WindowEventSubscriber(Graphics* gfx);
                ~WindowEventSubscriber();
        };
        WindowEventSubscriber::View eventSubscriber;
};

}

#endif // PGE_GRAPHICS_H_INCLUDED

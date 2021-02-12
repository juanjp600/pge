#ifndef PGE_GRAPHICS_H_INCLUDED
#define PGE_GRAPHICS_H_INCLUDED

#include <vector>

#include <Window/Window.h>
#include <Math/Rectangle.h>
#include <Color/Color.h>

namespace PGE {

class Texture;

class Graphics {
    public:
        enum class Renderer {
            OpenGL,
            DirectX11
        };

        static Graphics* create(Renderer r, String name, int w, int h, bool fs);

        Window* getWindow() const;

        virtual Renderer getRenderer() = 0;
        
        virtual void update();
        void swap(bool vsync = true);

        virtual void clear(Color color) = 0;

        virtual void setDepthTest(bool enabled) = 0;
        virtual bool getDepthTest() const = 0;

        virtual void setRenderTarget(Texture* renderTarget) = 0;
        virtual void setRenderTargets(std::vector<Texture*> renderTargets) = 0;
        virtual void resetRenderTarget() = 0;

        virtual void setViewport(Rectanglei vp);
        Rectanglei getViewport() const;

        virtual ~Graphics(){};

    protected:
        virtual void cleanup() = 0;
        virtual void throwException(String func, String details) = 0;

        Rectanglei viewport;

        Window* window;
};

}

#endif // PGE_GRAPHICS_H_INCLUDED

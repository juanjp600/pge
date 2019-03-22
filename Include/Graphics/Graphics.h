#ifndef PGE_GRAPHICS_H_INCLUDED
#define PGE_GRAPHICS_H_INCLUDED

#include <Window/Window.h>
#include <Math/Rectangle.h>
#include <Math/Matrix.h>
#include <Color/Color.h>

#include <vector>

namespace PGE {

class Texture;

class Graphics {
    public:
        static Graphics* create(String name,int w, int h, bool fs);

        Window* getWindow() const;
        
        virtual void update();
        void swap(bool vsync=true);

        virtual void clear(Color color) =0;

        virtual void setRenderTarget(Texture* renderTarget) =0;
        virtual void setRenderTargets(std::vector<Texture*> renderTargets) =0;
        virtual void resetRenderTarget() =0;

        virtual void setViewport(Rectanglei vp);
        Rectanglei getViewport() const;

        virtual ~Graphics(){};
    protected:
        Graphics();

        Rectanglei viewport;

        Window* window;
};

}

#endif

#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <Graphics/Graphics.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#endif

struct SDL_Window;

namespace PGE {

class GraphicsInternal : public Graphics {
    public:
        GraphicsInternal(String name, int w, int h, bool fs, uint32_t windowFlags);

        void setRenderer(Renderer r);
        Renderer getRenderer();

        SDL_Window* getSdlWindow() const;
#if defined(__APPLE__) && defined(__OBJC__)
        NSWindow* getCocoaWindow() const;
#endif

    private:
        // Hiding.
        using Graphics::renderer;
};

}

#endif // PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

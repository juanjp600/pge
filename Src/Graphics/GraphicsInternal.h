#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <SDL.h>

#include <PGE/Graphics/Graphics.h>
#include <PGE/Graphics/Mesh.h>
#include <PGE/Graphics/Texture.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#endif

namespace PGE {

class Shader;

class SDLWindow;

class GraphicsInternal : public Graphics {
    private:
        using Graphics::resourceManager;

    protected:
        static String appendInfoLine(const String& name, int value);
        static String appendInfoLine(const String& name, bool value);

#if defined(__APPLE__) && defined(__OBJC__)
        NSWindow* getCocoaWindow() const;
#endif

        // TODO: The more appropriate template solution is available in C++20.
        const String RENDERER_NAME;

        class SDLWindow : public Resource<SDL_Window*> {
            public:
                SDLWindow(const String& title, int width, int height, u32 flags);
                ~SDLWindow();
        };
        SDLWindow::View sdlWindow;

        GraphicsInternal(const String& rendererName, const String& name, int w, int h, bool fs, SDL_WindowFlags windowFlags);

    public:
        String getInfo() const override;

        virtual Shader* loadShader(const FilePath& path) = 0;
        virtual Mesh* createMesh() = 0;
        virtual Texture* createRenderTargetTexture(int w, int h, Texture::Format fmt) = 0;
        virtual Texture* loadTexture(int w, int h, const byte* buffer, Texture::Format fmt, bool mipmaps) = 0;

        SDL_Window* getWindow() const;
};

template <typename ShaderType, typename MeshType, typename TextureType>
class GraphicsSpecialized : public GraphicsInternal {
    static_assert(std::is_base_of<Shader, ShaderType>::value);
    static_assert(std::is_base_of<Mesh, MeshType>::value);
    static_assert(std::is_base_of<Texture, TextureType>::value);

    protected:
        GraphicsSpecialized(const String& rendererName, const String& name, int w, int h, bool fs, SDL_WindowFlags windowFlags)
            : GraphicsInternal(rendererName, name, w, h, fs, windowFlags) { }

    public:
        Shader* loadShader(const FilePath& path) final override {
            return new ShaderType(*this, path);
        }

        Mesh* createMesh() final override {
            return new MeshType(*this);
        }

        Texture* createRenderTargetTexture(int w, int h, Texture::Format fmt) final override {
            return new TextureType(*this, w, h, fmt);
        }

        Texture* loadTexture(int w, int h, const byte* buffer, Texture::Format fmt, bool mipmaps) final override {
            return new TextureType(*this, w, h, buffer, fmt, mipmaps);
        }
};

template <typename SPEC_GFX>
class GraphicsReferencer {
    protected:
        GraphicsReferencer(Graphics& gfx) : graphics((SPEC_GFX&)gfx) { validate(); }
        GraphicsReferencer(const Graphics& gfx) : graphics((SPEC_GFX&)gfx) { validate(); }

        SPEC_GFX& graphics;

    private:
        constexpr void validate() {
            static_assert(std::is_base_of<Graphics, SPEC_GFX>::value);
        }
};

}

#endif // PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

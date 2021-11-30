#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <SDL.h>

#include <PGE/Graphics/Graphics.h>
#include <PGE/Graphics/Mesh.h>
#include <PGE/Graphics/Texture.h>
#include <PGE/Graphics/Material.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#endif

namespace PGE {

class Shader;

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
                SDLWindow(const String& title, int x, int y, int width, int height, u32 flags);
                ~SDLWindow();
        };
        SDLWindow::View sdlWindow;

        GraphicsInternal(const String& rendererName, const String& name, int w, int h, WindowMode wm, int x, int y, SDL_WindowFlags windowFlags);

    public:
        String getInfo() const override;

        virtual Shader* loadShader(const FilePath& path) = 0;
        virtual Mesh* createMesh() = 0;
        virtual Texture* createRenderTargetTexture(int w, int h, Texture::Format fmt) = 0;
        virtual Texture* loadTexture(int w, int h, const byte* buffer, Texture::Format fmt, bool mipmaps) = 0;
        virtual Texture* loadTextureCompressed(const std::vector<Texture::Mipmap>& mipmaps, Texture::CompressedFormat fmt) = 0;
        virtual Material* createMaterial(Shader& sh, const ReferenceVector<Texture>& tex, Opaque o) = 0;

        SDL_Window* getWindow() const;
};

template <std::derived_from<Shader> SHADER, std::derived_from<Mesh> MESH, std::derived_from<Texture> TEXTURE,
    std::derived_from<Material> MATERIAL = Material, std::derived_from<Texture> RENDER_TEXTURE = TEXTURE>
class GraphicsSpecialized : public GraphicsInternal {
    protected:
        GraphicsSpecialized(const String& rendererName, const String& name, int w, int h, WindowMode wm, int x, int y, SDL_WindowFlags windowFlags)
            : GraphicsInternal(rendererName, name, w, h, wm, x, y, windowFlags) { }

    public:
        Shader* loadShader(const FilePath& path) final override {
            return new SHADER(*this, path);
        }

        Mesh* createMesh() final override {
            return new MESH(*this);
        }

        Texture* createRenderTargetTexture(int w, int h, Texture::Format fmt) final override {
            return new RENDER_TEXTURE(*this, w, h, fmt);
        }

        Texture* loadTexture(int w, int h, const byte* buffer, Texture::Format fmt, bool mipmaps) final override {
            return new TEXTURE(*this, w, h, buffer, fmt, mipmaps);
        }
        
        Texture* loadTextureCompressed(const std::vector<Texture::Mipmap>& mipmaps, Texture::CompressedFormat fmt) final override {
            return new TEXTURE(*this, mipmaps, fmt);
        }

        Material* createMaterial(Shader& sh, const ReferenceVector<Texture>& tex, Opaque o) final override {
            return new MATERIAL(*this, sh, tex, o);
        }
};

}

#endif // PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

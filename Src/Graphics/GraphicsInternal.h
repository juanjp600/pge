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
        virtual Material* createMaterial(Shader& sh, const ReferenceVector<Texture>& tex, Material::Opaque o) = 0;

        SDL_Window* getWindow() const;
};

template <typename ShaderType, typename MeshType, typename TextureType, typename MaterialType = Material, typename RenderTexture = TextureType>
class GraphicsSpecialized : public GraphicsInternal {
    static_assert(std::is_base_of<Shader, ShaderType>::value);
    static_assert(std::is_base_of<Mesh, MeshType>::value);
    static_assert(std::is_base_of<Texture, TextureType>::value);

    protected:
        GraphicsSpecialized(const String& rendererName, const String& name, int w, int h, WindowMode wm, int x, int y, SDL_WindowFlags windowFlags)
            : GraphicsInternal(rendererName, name, w, h, wm, x, y, windowFlags) { }

    public:
        Shader* loadShader(const FilePath& path) final override {
            return new ShaderType(*this, path);
        }

        Mesh* createMesh() final override {
            return new MeshType(*this);
        }

        Texture* createRenderTargetTexture(int w, int h, Texture::Format fmt) final override {
            return new RenderTexture(*this, w, h, fmt);
        }

        Texture* loadTexture(int w, int h, const byte* buffer, Texture::Format fmt, bool mipmaps) final override {
            return new TextureType(*this, w, h, buffer, fmt, mipmaps);
        }
        
        Texture* loadTextureCompressed(const std::vector<Texture::Mipmap>& mipmaps, Texture::CompressedFormat fmt) final override {
            return new TextureType(*this, mipmaps, fmt);
        }

        Material* createMaterial(Shader& sh, const ReferenceVector<Texture>& tex, Material::Opaque o) final override {
            return new MaterialType(*this, sh, tex, o);
        }
};

}

#endif // PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <PGE/Graphics/Graphics.h>
#include <PGE/Graphics/Mesh.h>
#include <PGE/Graphics/Texture.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#endif

#define APPEND(name) '\n' + #name + ": " + String::fromInt(name)

struct SDL_Window;

namespace PGE {

class Shader;
class ThreadManager;

class GraphicsInternal : public Graphics {
    public:
        using Graphics::Graphics;
        using Graphics::getWindow;

        virtual Shader* loadShader(const FilePath& path) = 0;
        virtual Mesh* createMesh(Primitive::Type pt) = 0;
        virtual Texture* createRenderTargetTexture(int w, int h, Texture::Format fmt) = 0;
        virtual Texture* loadTexture(int w, int h, const byte* buffer, Texture::Format fmt, bool mipmaps) = 0;
};

template <class SHADER, class MESH, class TEXTURE>
class GraphicsSpecialized : public GraphicsInternal {
    private:
        static const inline String RENDERER_NAME = "SHIT";

    public:
        using GraphicsInternal::GraphicsInternal;
        using Graphics::getWindow;

#if defined(__APPLE__) && defined(__OBJC__)
        NSWindow* getCocoaWindow() const;
#endif

        String getInfo() const override {
            return caption + " (" + RENDERER_NAME + ") "
                + String::fromInt(dimensions.x) + 'x' + String::fromInt(dimensions.y) + " / "
                + String::fromInt(viewport.width()) + 'x' + String::fromInt(viewport.height())
                + APPEND(open) + APPEND(focused) + APPEND(fullscreen) + APPEND(vsync) + APPEND(depthTest);
        }

        Shader* loadShader(const FilePath& path) final override {
            static_assert(std::is_base_of<Shader, SHADER>::value);
            return new SHADER(this, path);
        }

        Mesh* createMesh(Primitive::Type pt) final override {
            static_assert(std::is_base_of<Mesh, MESH>::value);
            return new MESH(this, pt);
        }

        Texture* createRenderTargetTexture(int w, int h, Texture::Format fmt) final override {
            static_assert(std::is_base_of<Texture, TEXTURE>::value);
            return new TEXTURE(this, w, h, fmt);
        }

        Texture* loadTexture(int w, int h, const byte* buffer, Texture::Format fmt, bool mipmaps) final override {
            static_assert(std::is_base_of<Texture, TEXTURE>::value);
            return new TEXTURE(this, w, h, buffer, fmt, mipmaps);
        }
};

}

#endif // PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

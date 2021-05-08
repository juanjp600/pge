#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <Graphics/Graphics.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#endif

#include <Misc/FilePath.h>
#include <Mesh/Mesh.h>
#include <Texture/Texture.h>

#define PGE_GFX_OBJ_DEC \
Shader* loadShader(const FilePath& path) override; \
Mesh* createMesh(Primitive::TYPE pt) override; \
Texture* createRenderTargetTexture(int w, int h, Texture::FORMAT fmt) override; \
Texture* loadTexture(int w, int h, uint8_t* buffer, Texture::FORMAT fmt) override; \
String getRendererName() const override;

#define PGE_GFX_OBJ_DEF(Type) \
Shader* Graphics ## Type ## ::loadShader(const FilePath& path) { \
    return new Shader ## Type ## (this, path); \
} \
\
Mesh* Graphics ## Type ## ::createMesh(Primitive::TYPE pt) { \
    return new Mesh ## Type ## (this, pt); \
} \
\
Texture* Graphics ## Type ## ::createRenderTargetTexture(int w, int h, Texture::FORMAT fmt) { \
    return new Texture ## Type ## (this, w, h, fmt); \
} \
\
Texture* Graphics ## Type ## ::loadTexture(int w, int h, uint8_t* buffer, Texture::FORMAT fmt) { \
    return new Texture ## Type ## (this, w, h, buffer, fmt); \
} \
\
String Graphics ## Type ## ::getRendererName() const { \
    return #Type; \
}

struct SDL_Window;

namespace PGE {

class Shader;
class ThreadManager;

class GraphicsInternal : public Graphics {
    public:
        using Graphics::Graphics;

        SDL_Window* getSdlWindow() const;
#if defined(__APPLE__) && defined(__OBJC__)
        NSWindow* getCocoaWindow() const;
#endif

        virtual Shader* loadShader(const FilePath& path) = 0;
        virtual Mesh* createMesh(Primitive::TYPE pt) = 0;
        virtual Texture* createRenderTargetTexture(int w, int h, Texture::FORMAT fmt) = 0;
        virtual Texture* loadTexture(int w, int h, uint8_t* buffer, Texture::FORMAT fmt) = 0;
};

}

#endif // PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

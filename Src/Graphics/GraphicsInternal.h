#ifndef PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED
#define PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

#include <Graphics/Graphics.h>

#if defined(__APPLE__) && defined(__OBJC__)
#import <AppKit/AppKit.h>
#endif

#include <Misc/FilePath.h>
#include <Mesh/Mesh.h>
#include <Texture/Texture.h>

#define __GFX_OBJ_DEC \
Shader* loadShader(const FilePath& path) override; \
Mesh* createMesh(Primitive::TYPE pt) override; \
Texture* loadTexture(uint8_t* buffer, int w, int h, int rw, int rh, const FilePath& fn = FilePath::fromStr("")) override; \
Texture* loadTexture(const FilePath& filename, ThreadManager* threadManager) override; \
Texture* createTexture(int w, int h, bool renderTarget, const void* buffer, Texture::FORMAT fmt) override;

#define __GFX_OBJ_DEF(Type) \
Shader* Graphics ## Type ## ::loadShader(const FilePath& path) { \
    return new Shader ## Type ## (this, path); \
} \
\
Mesh* Graphics ## Type ## ::createMesh(Primitive::TYPE pt) { \
    return new Mesh ## Type ## (this, pt); \
} \
\
Texture* Graphics ## Type ## ::loadTexture(uint8_t* buffer, int w, int h, int rw, int rh, const FilePath& fn) { \
    return new Texture ## Type ## (this, buffer, w, h, rw, rh, fn); \
} \
\
Texture* Graphics ## Type ## ::loadTexture(const FilePath& filename, ThreadManager* threadManager) { \
    return new Texture ## Type ## (this, filename, threadManager); \
} \
\
Texture* Graphics ## Type ## ::createTexture(int w, int h, bool renderTarget, const void* buffer, Texture::FORMAT fmt) { \
    return new Texture ## Type ## (this, w, h, renderTarget, buffer, fmt); \
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
        virtual Texture* loadTexture(uint8_t* buffer, int w, int h, int rw, int rh, const FilePath& fn) = 0;
        virtual Texture* loadTexture(const FilePath& filename, ThreadManager* threadManager) = 0;
        virtual Texture* createTexture(int w, int h, bool renderTarget, const void* buffer, Texture::FORMAT fmt) = 0;
};

}

#endif // PGEINTERNAL_WINDOWINTERNAL_H_INCLUDED

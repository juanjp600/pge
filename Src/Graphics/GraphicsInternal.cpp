#include "GraphicsInternal.h"
#include "GraphicsDX11.h"
#include "GraphicsOGL3.h"
#include "../Shader/ShaderDX11.h"
#include "../Shader/ShaderOGL3.h"
#include "../Mesh/MeshDX11.h"
#include "../Mesh/MeshOGL3.h"
#include "../Texture/TextureDX11.h"
#include "../Texture/TextureOGL3.h"
#include "../Texture/TextureInternal.h"

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#include <SDL_syswm.h>
#endif

using namespace PGE;

SDL_Window* GraphicsInternal::getSdlWindow() const {
    return sdlWindow;
}

#if defined(__APPLE__) && defined(__OBJC__)
NSWindow* GraphicsInternal::getCocoaWindow() const {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(getSdlWindow(), &info);

    return info.info.cocoa.window;
}
#endif

Graphics* Graphics::create(String name, int w, int h, bool fs, Renderer r) {
    if (r == Renderer::Default) {
#ifdef WINDOWS
        r = Renderer::DirectX11;
#else
        r = Renderer::OpenGL;
#endif
    }
    Graphics* gfx;
    switch (r) {
        case Renderer::DirectX11: {
            gfx = new GraphicsDX11(name, w, h, fs);
        } break;
        case Renderer::OpenGL: {
            gfx = new GraphicsOGL3(name, w, h, fs);
        } break;
        default: {
            gfx = nullptr;
        } break;
    }
    return gfx;
}

Shader* Shader::load(Graphics* gfx, const FilePath& path) {
    return ((GraphicsInternal*)gfx)->loadShader(path);
}

Mesh* Mesh::create(Graphics* gfx, Primitive::TYPE pt) {
    return ((GraphicsInternal*)gfx)->createMesh(pt);
}

Texture* Texture::load(Graphics* gfx, const void* buffer, int size) {
    int width; int height;
    int realWidth; int realHeight;
    std::unique_ptr<BYTE> fiBuffer = std::unique_ptr<BYTE>(loadFIBufferFromMemory(buffer, size, width, height, realWidth, realHeight));
    return ((GraphicsInternal*)gfx)->loadTexture(fiBuffer.get(), width, height, realWidth, realHeight, FilePath::fromStr("_:memory")); // TODO: Better solution for path
}

Texture* Texture::load(Graphics* gfx, const FilePath& filename) {
    int width; int height;
    int realWidth; int realHeight;
    std::unique_ptr<BYTE> fiBuffer = std::unique_ptr<BYTE>(loadFIBufferFromFile(filename, width, height, realWidth, realHeight));
    return ((GraphicsInternal*)gfx)->loadTexture(fiBuffer.get(), width, height, realWidth, realHeight, filename);
}

Texture* Texture::load(Graphics* gfx, const FilePath& filename, ThreadManager* threadManager) {
    return ((GraphicsInternal*)gfx)->loadTexture(filename, threadManager);
}

Texture* Texture::create(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer, FORMAT fmt) {
    return ((GraphicsInternal*)gfx)->createTexture(w, h, renderTarget, buffer, fmt);
}

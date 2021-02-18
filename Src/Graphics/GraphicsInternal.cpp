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

GraphicsInternal::GraphicsInternal(String name, int w, int h, bool fs) : Graphics(name, w, h, fs) {}

void GraphicsInternal::setRenderer(Renderer r) {
    renderer = r;
}

void GraphicsInternal::setRendererName(String name) {
    rendererName = name;
}

Graphics::Renderer GraphicsInternal::getRenderer() {
    return renderer;
}

SDL_Window* GraphicsInternal::getSdlWindow() const {
    return sdlWindow.get();
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
    GraphicsInternal* gfx;
    switch (r) {
        case Renderer::DirectX11: {
            gfx = new GraphicsDX11(name, w, h, fs);
            gfx->setRendererName("GraphicsDX11");
        } break;
        case Renderer::OpenGL: {
            gfx = new GraphicsOGL3(name, w, h, fs);
            gfx->setRendererName("GraphicsOGL3");
        } break;
        default: {
            gfx = nullptr;
        } break;
    }
    gfx->setRenderer(r);
    return gfx;
}

Shader* Shader::load(Graphics* gfx, const FilePath& path) {
    switch (((GraphicsInternal*)gfx)->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new ShaderDX11(gfx, path);
        }
        case Graphics::Renderer::OpenGL: {
            return new ShaderOGL3(gfx, path);
        }
        default: {
            return nullptr;
        }
    }
}

Mesh* Mesh::create(Graphics* gfx, Primitive::TYPE pt) {
    switch (((GraphicsInternal*)gfx)->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new MeshDX11(gfx, pt);
        }
        case Graphics::Renderer::OpenGL: {
            return new MeshOGL3(gfx, pt);
        }
        default: {
            return nullptr;
        }
    }
}

Texture* Texture::load(Graphics* gfx, const void* buffer, int size) {
    BYTE* fiBuffer = nullptr;
    int width; int height;
    int realWidth; int realHeight;
    try {
        fiBuffer = loadFIBufferFromMemory(buffer, size, width, height, realWidth, realHeight);
    } catch (Exception& e) {
        delete[] fiBuffer;
        throw e;
    } catch (std::exception& e) {
        delete[] fiBuffer;
        throw e;
    }
    switch (((GraphicsInternal*)gfx)->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new TextureDX11(gfx, fiBuffer, width, height, realWidth, realHeight);
        }
        case Graphics::Renderer::OpenGL: {
            return new TextureOGL3(gfx, fiBuffer, width, height, realWidth, realHeight);
        }
        default: {
            return nullptr;
        }
    }
    delete[] fiBuffer;
}

Texture* Texture::load(Graphics* gfx, const FilePath& filename) {
    BYTE* fiBuffer = nullptr;
    int width; int height;
    int realWidth; int realHeight;
    try {
        fiBuffer = loadFIBufferFromFile(filename, width, height, realWidth, realHeight);
    } catch (Exception& e) {
        delete[] fiBuffer;
        throw e;
    } catch (std::exception& e) {
        delete[] fiBuffer;
        throw e;
    }
    switch (((GraphicsInternal*)gfx)->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new TextureDX11(gfx, fiBuffer, width, height, realWidth, realHeight);
        }
        case Graphics::Renderer::OpenGL: {
            return new TextureOGL3(gfx, fiBuffer, width, height, realWidth, realHeight);
        }
        default: {
            return nullptr;
        }
    }
    delete[] fiBuffer;
}

Texture* Texture::load(Graphics* gfx, const FilePath& filename, ThreadManager* threadManager) {
    switch (((GraphicsInternal*)gfx)->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new TextureDX11(gfx, filename, threadManager);
        }
        case Graphics::Renderer::OpenGL: {
            return new TextureOGL3(gfx, filename, threadManager);
        }
        default: {
            return nullptr;
        }
    }
}

Texture* Texture::create(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer, FORMAT fmt) {
    switch (((GraphicsInternal*)gfx)->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new TextureDX11(gfx, w, h, renderTarget, buffer, fmt);
        }
        case Graphics::Renderer::OpenGL: {
            return new TextureOGL3(gfx, w, h, renderTarget, buffer, fmt);
        }
        default: {
            return nullptr;
        }
    }
}

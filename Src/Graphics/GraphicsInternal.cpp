#ifdef _WIN32
#include "GraphicsDX11.h"
#endif
#include "GraphicsOGL3.h"
#include "GraphicsVK.h"

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#include <PGE/SDL_syswm.h>
#endif

using namespace PGE;

String GraphicsInternal::appendInfoLine(const String& name, int value) {
    return "\n" + name + ": " + String::from(value);
}

String GraphicsInternal::appendInfoLine(const String& name, bool value) {
    return "\n" + name + ": " + (value ? "true" : "false");
}

#if defined(__APPLE__) && defined(__OBJC__)
NSWindow* GraphicsInternal::getCocoaWindow() const {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(getSdlWindow(), &info);

    return info.info.cocoa.window;
}
#endif

GraphicsInternal::GraphicsInternal(const String& rendererName, const String& name, int w, int h, WindowMode wm, int x, int y, SDL_WindowFlags windowFlags)
    : Graphics(name, w, h, wm), RENDERER_NAME(rendererName) {
    sdlWindow = resourceManager.addNewResource<SDLWindow>(name, x, y, w, h, windowFlags);
}

String GraphicsInternal::getInfo() const {
    return caption + " (" + RENDERER_NAME + ") "
        + String::from(dimensions.x) + 'x' + String::from(dimensions.y) + " / "
        + String::from(viewport.width()) + 'x' + String::from(viewport.height())
        + appendInfoLine("open", open)
        + appendInfoLine("focused", focused)
        + appendInfoLine("windowMode", windowMode == WindowMode::Fullscreen ? "Fullscreen" : "Windowed")
        + appendInfoLine("vsync enabled", vsync)
        + appendInfoLine("depth test enabled", depthTest);
}

SDL_Window* GraphicsInternal::getWindow() const {
    return sdlWindow;
}

GraphicsInternal::SDLWindow::SDLWindow(const String& title, int x, int y, int width, int height, u32 flags) {
    resource = SDL_CreateWindow(title.cstr(), x, y, width, height, flags);
    PGE_ASSERT(resource != nullptr, "Failed to create SDL window (SDLERROR: " + String(SDL_GetError()) + ")");
}

GraphicsInternal::SDLWindow::~SDLWindow() {
    SDL_DestroyWindow(resource);
}

Graphics* Graphics::create(const String& name, int w, int h, WindowMode wm, std::optional<Renderer> r, int x, int y) {
    if (!r.has_value()) {
#ifdef _WIN32
        r = Renderer::DirectX11;
#else
        r = Renderer::OpenGL;
#endif
    }
    Graphics* gfx;
    switch (r.value()) {
#ifdef _WIN32
        case Renderer::DirectX11: {
            gfx = new GraphicsDX11(name, w, h, wm, x, y);
        } break;
#endif
        case Renderer::OpenGL: {
            gfx = new GraphicsOGL3(name, w, h, wm, x, y);
        } break;
        case Renderer::Vulkan: {
            gfx = new GraphicsVK(name, w, h, wm, x, y);
        } break;
        default: {
            gfx = nullptr;
        } break;
    }
    return gfx;
}

Shader* Shader::load(Graphics& gfx, const FilePath& path) {
    return ((GraphicsInternal&)gfx).loadShader(path.makeDirectory());
}

Mesh* Mesh::create(Graphics& gfx) {
    return ((GraphicsInternal&)gfx).createMesh();
}

Texture* Texture::createRenderTarget(Graphics& gfx, int w, int h, Format fmt) {
    return ((GraphicsInternal&)gfx).createRenderTargetTexture(w, h, fmt);
}

Texture* Texture::createBlank(Graphics& gfx, int w, int h, Format fmt, bool mipmaps) {
    std::vector<byte> bufferData(w * h * 4, 0);
    return ((GraphicsInternal&)gfx).loadTexture(w, h, bufferData.data(), fmt, mipmaps);
}

Texture* Texture::load(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps) {
    PGE_ASSERT(buffer != nullptr, "Tried to load texture from nullptr");
    return ((GraphicsInternal&)gfx).loadTexture(w, h, buffer, fmt, mipmaps);
}

Texture* Texture::loadCompressed(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt) {
    return ((GraphicsInternal&)gfx).loadTextureCompressed(mipmaps, fmt);
}

#include "GraphicsInternal.h"
#include "GraphicsDX11.h"
#include "GraphicsOGL3.h"
#include "Shader/ShaderDX11.h"
#include "Shader/ShaderOGL3.h"
#include "Mesh/MeshDX11.h"
#include "Mesh/MeshOGL3.h"
#include "Texture/TextureDX11.h"
#include "Texture/TextureOGL3.h"

#if defined(__APPLE__) && defined(__OBJC__)
#import <Foundation/Foundation.h>
#include <PGE/SDL_syswm.h>
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

Graphics* Graphics::create(const String& name, int w, int h, bool fs, Renderer r) {
    if (r == Renderer::Default) {
#ifdef _WIN32
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

Mesh* Mesh::create(Graphics* gfx, Primitive::Type pt) {
    return ((GraphicsInternal*)gfx)->createMesh(pt);
}

Texture* Texture::createRenderTarget(Graphics* gfx, int w, int h, Format fmt) {
    return ((GraphicsInternal*)gfx)->createRenderTargetTexture(w, h, fmt);
}

Texture* Texture::createBlank(Graphics* gfx, int w, int h, Format fmt) {
    std::vector<byte> bufferData = std::vector<byte>(w * h * 4, 0);
    return ((GraphicsInternal*)gfx)->loadTexture(w, h, bufferData.data(), fmt);
}

Texture* Texture::load(Graphics* gfx, int w, int h, byte* buffer, Format fmt) {
    PGE_ASSERT(buffer != nullptr, "Tried to load texture from nullptr");
    return ((GraphicsInternal*)gfx)->loadTexture(w, h, buffer, fmt);
}

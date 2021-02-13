#include "GraphicsDX11.h"
#include "GraphicsOGL3.h"
#include "GraphicsVK.h"
#include "../Shader/ShaderDX11.h"
#include "../Shader/ShaderOGL3.h"
#include "../Shader/ShaderVK.h"
#include "../Mesh/MeshDX11.h"
#include "../Mesh/MeshOGL3.h"
#include "../Mesh/MeshVK.h"
#include "../Texture/TextureDX11.h"
#include "../Texture/TextureOGL3.h"
#include "../Texture/TextureVK.h"
#include "../Texture/TextureInternal.h"

using namespace PGE;

Graphics* Graphics::create(Renderer r, String name, int w, int h, bool fs) {
    switch (r) {
        case Renderer::DirectX11: {
            return new GraphicsDX11(name, w, h, fs);
            break;
        }
        case Renderer::OpenGL: {
            return new GraphicsOGL3(name, w, h, fs);
            break;
        }
        case Renderer::Vulkan: {
            return new GraphicsVK(name, w, h, fs);
        }
    }
}

Shader* Shader::load(Graphics* gfx, const FilePath& path) {
    switch (gfx->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new ShaderDX11(gfx, path);
            break;
        }
        case Graphics::Renderer::OpenGL: {
            return new ShaderOGL3(gfx, path);
            break;
        }
        case Graphics::Renderer::Vulkan: {
            return new ShaderVK(gfx, path);
        }
    }
}

Mesh* Mesh::create(Graphics* gfx, Primitive::TYPE pt) {
    switch (gfx->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new MeshDX11(gfx, pt);
            break;
        }
        case Graphics::Renderer::OpenGL: {
            return new MeshOGL3(gfx, pt);
            break;
        }
        case Graphics::Renderer::Vulkan: {
            return new MeshVK(gfx, pt);
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
    switch (gfx->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new TextureDX11(gfx, fiBuffer, width, height, realWidth, realHeight);
            break;
        }
        case Graphics::Renderer::OpenGL: {
            return new TextureOGL3(gfx, fiBuffer, width, height, realWidth, realHeight);
            break;
        }
        case Graphics::Renderer::Vulkan: {
            return new TextureVK(gfx, fiBuffer, width, height, realWidth, realHeight);
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
    switch (gfx->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new TextureDX11(gfx, fiBuffer, width, height, realWidth, realHeight);
            break;
        }
        case Graphics::Renderer::OpenGL: {
            return new TextureOGL3(gfx, fiBuffer, width, height, realWidth, realHeight);
            break;
        }
        case Graphics::Renderer::Vulkan: {
            return new TextureVK(gfx, fiBuffer, width, height, realWidth, realHeight);
        }
    }
    delete[] fiBuffer;
}

Texture* Texture::load(Graphics* gfx, const FilePath& filename, ThreadManager* threadManager) {
    switch (gfx->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new TextureDX11(gfx, filename, threadManager);
            break;
        }
        case Graphics::Renderer::OpenGL: {
            return new TextureOGL3(gfx, filename, threadManager);
            break;
        }
        case Graphics::Renderer::Vulkan: {
            return new TextureVK(gfx, filename, threadManager);
        }
    }
}

Texture* Texture::create(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer, FORMAT fmt) {
    switch (gfx->getRenderer()) {
        case Graphics::Renderer::DirectX11: {
            return new TextureDX11(gfx, w, h, renderTarget, buffer, fmt);
            break;
        }
        case Graphics::Renderer::OpenGL: {
            return new TextureOGL3(gfx, w, h, renderTarget, buffer, fmt);
            break;
        }
        case Graphics::Renderer::Vulkan: {
            return new TextureVK(gfx, w, h, renderTarget, buffer, fmt);
        }
    }
}

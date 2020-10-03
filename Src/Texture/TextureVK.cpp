#include "TextureVK.h"

using namespace PGE;

#ifdef VULKAN
Texture* Texture::load(Graphics* gfx, const FilePath& filename) {
    return new TextureVK(gfx, filename);
}

Texture* Texture::load(Graphics* gfx, const FilePath& filename, ThreadManager* threadManager) {
    return new TextureVK(gfx, filename, threadManager);
}

Texture* Texture::create(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer, FORMAT fmt) {
    return new TextureVK(gfx, w, h, renderTarget, buffer, fmt);
}
#endif

TextureVK::TextureVK(Graphics* gfx, int width, int height, bool renderTarget, const void* buffer, FORMAT fmt) {

}

TextureVK::TextureVK(Graphics* gfx, const FilePath& fn) {

}

TextureVK::TextureVK(Graphics* gfx, const FilePath& fn, ThreadManager* threadManager) {
    
}

TextureVK::~TextureVK() {

}

bool TextureVK::isRenderTarget() const {
    return false;
}

void* TextureVK::getNative() const {
    return nullptr;
}

void TextureVK::cleanup() {

}

void TextureVK::throwException(String func, String details) {

}

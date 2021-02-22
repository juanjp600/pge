#include "TextureVK.h"

using namespace PGE;

TextureVK::TextureVK(Graphics* gfx, int width, int height, bool renderTarget, const void* buffer, FORMAT fmt) {

}

TextureVK::TextureVK(Graphics* gfx, const FilePath& fn, ThreadManager* threadManager) {
    
}

TextureVK::TextureVK(Graphics* gfx, uint8_t* fiBuffer, int w, int h, int rw, int rh, const FilePath& fn) {

}

TextureVK::~TextureVK() {

}

Texture* TextureVK::copy() const {
    return (Texture*)this;
}

void* TextureVK::getNative() const {
    return nullptr;
}

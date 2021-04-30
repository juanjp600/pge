#include "TextureVK.h"

using namespace PGE;

TextureVK::TextureVK(Graphics* gfx, int w, int h, FORMAT fmt) : Texture(gfx, w, h, true, fmt) {

}

TextureVK::TextureVK(Graphics* gfx, int w, int h, uint8_t* buffer, FORMAT fmt) : Texture(gfx, w, h, false, fmt) {

}

Texture* TextureVK::copy() const {
    return (Texture*)this;
}

void* TextureVK::getNative() const {
    return nullptr;
}

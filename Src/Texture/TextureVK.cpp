#include "TextureVK.h"

using namespace PGE;

TextureVK::TextureVK(Graphics* gfx, int w, int h, Format fmt) : Texture(gfx, w, h, true, fmt) {

}

TextureVK::TextureVK(Graphics* gfx, int w, int h, uint8_t* buffer, Format fmt) : Texture(gfx, w, h, false, fmt) {

}

Texture* TextureVK::copy() const {
    return (Texture*)this;
}

void* TextureVK::getNative() const {
    return nullptr;
}

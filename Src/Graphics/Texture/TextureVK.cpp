#include "TextureVK.h"

using namespace PGE;


TextureVK::TextureVK(Graphics& gfx, int w, int h, Format fmt) : Texture(w, h, true, fmt) {

}

TextureVK::TextureVK(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps) : Texture(w, h, false, fmt) {

}

TextureVK::TextureVK(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt) : Texture(mipmaps[0].width, mipmaps[0].height, false, fmt) {

}

void* TextureVK::getNative() const {
    return nullptr;
}

#include <Texture/Texture.h>

using namespace PGE;

Texture::Texture(Graphics* gfx, int w, int h, bool rt, FORMAT fmt) {
    graphics = gfx;
    width = w;
    height = h;
    isRT = rt;
    format = fmt;
}

bool Texture::isRenderTarget() const {
    return isRT;
}

int Texture::getWidth() const {
    return width;
}

int Texture::getHeight() const {
    return height;
}

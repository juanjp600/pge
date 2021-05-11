#include <Texture/Texture.h>

using namespace PGE;

Texture::Texture(Graphics* gfx, int w, int h, bool rt, Format fmt) {
    graphics = gfx;
    dimensions = Vector2i(w,h);
    isRT = rt;
    format = fmt;
}

bool Texture::isRenderTarget() const {
    return isRT;
}

int Texture::getWidth() const {
    return dimensions.x;
}

int Texture::getHeight() const {
    return dimensions.y;
}

Vector2i Texture::getDimensions() const {
    return dimensions;
}

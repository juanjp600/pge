#include <PGE/Graphics/Texture.h>

#include <PGE/Exception/Exception.h>

using namespace PGE;

Texture::Texture(int w, int h, bool rt, const AnyFormat& fmt)
    : dimensions(w, h), isRT(rt), format(fmt) { }

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

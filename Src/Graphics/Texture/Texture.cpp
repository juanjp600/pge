#include <PGE/Graphics/Texture.h>

#include <PGE/Exception/Exception.h>

using namespace PGE;

int Texture::getBytesPerPixel(Format fmt) {
    switch (fmt) {
        case Format::RGBA64: {
            return 8;
        }
        case Format::RGBA32:
        case Format::R32F: {
            return 4;
        }
        case Format::R8: {
            return 1;
        }
        default: {
            throw PGE_CREATE_EX("Invalid format");
        }
    }
}

Texture::Texture(int w, int h, bool rt, Format fmt)
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

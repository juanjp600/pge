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

int Texture::getPixelsPerBlock(CompressedFormat fmt) {
    switch (fmt) {
        case CompressedFormat::BC1:
        case CompressedFormat::BC4: {
            return 2;
        }
        case CompressedFormat::BC2:
        case CompressedFormat::BC3:
        case CompressedFormat::BC5:
        case CompressedFormat::BC6:
        case CompressedFormat::BC7: {
            return 4;
        }
    }
}

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

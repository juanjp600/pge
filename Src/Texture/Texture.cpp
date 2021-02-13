#include <Texture/Texture.h>
#include "TextureInternal.h"
#include <Exception/Exception.h>

using namespace PGE;

bool Texture::isRenderTarget() const {
    return isRT;
}

int Texture::getWidth() const {
    return width;
}

int Texture::getHeight() const {
    return height;
}

static BYTE* load(FIBITMAP* data, const String& name, int& width, int& height, int& realWidth, int& realHeight) {
    if (data == nullptr) {
        throw Exception("loadFIBuffer", "Failed to load " + name);
    }
    FIBITMAP* image = FreeImage_ConvertTo32Bits(data);
    if (image == nullptr) {
        throw Exception("loadFIBuffer", "Failed to convert " + name + " to 32-bit RGBA");
    }
    FreeImage_Unload(data);

    int w = FreeImage_GetWidth(image);
    int h = FreeImage_GetHeight(image);

    width = w; height = h;

    int po2w = 1; int po2h = 1;
    while (po2w < w) { po2w *= 2; }
    while (po2h < h) { po2h *= 2; }

    if (w != po2w || h != po2h) {
        data = image;
        image = FreeImage_Rescale(data, po2w, po2h, FILTER_BILINEAR);
        if (image == nullptr) {
            throw Exception("loadFIBuffer", "Failed to rescale " + name + " (" + String::fromInt(w) + "," + String::fromInt(h) + " to " + String::fromInt(po2w) + "," + String::fromInt(po2h) + ")");
        }
        FreeImage_Unload(data);
    }

    realWidth = po2w; realHeight = po2h;

    BYTE* bits = FreeImage_GetBits(image);
    if (bits == nullptr) {
        throw Exception("loadFIBuffer", "Failed to retrieve data from " + name);
    }
    int bpp = FreeImage_GetBPP(image) / 8;
    BYTE* newBits = new BYTE[realWidth * realHeight * bpp];

    for (int x = 0; x < realWidth; x++) {
        for (int y = 0; y < realHeight; y++) {
            int invY = realHeight - 1 - y;

            //flip vertically and convert from bgra to rgba
            newBits[(x + (y * realWidth)) * bpp + 0] = bits[(x + (invY * realWidth)) * bpp + 2];
            newBits[(x + (y * realWidth)) * bpp + 1] = bits[(x + (invY * realWidth)) * bpp + 1];
            newBits[(x + (y * realWidth)) * bpp + 2] = bits[(x + (invY * realWidth)) * bpp + 0];
            newBits[(x + (y * realWidth)) * bpp + 3] = bits[(x + (invY * realWidth)) * bpp + 3];
        }
    }

    FreeImage_Unload(image);

    return newBits;
}

BYTE* PGE::loadFIBufferFromMemory(const void* buffer, int size, int& width, int& height, int& realWidth, int& realHeight) {
    FIMEMORY* stream = FreeImage_OpenMemory((BYTE*)buffer, size);
    BYTE* bytes = load(FreeImage_LoadFromMemory(FIF_PNG, stream), "from memory", width, height, realWidth, realHeight);
    FreeImage_CloseMemory(stream);
    return bytes;
}

BYTE* PGE::loadFIBufferFromFile(const FilePath& filename, int& width, int& height, int& realWidth, int& realHeight) {
    return load(FreeImage_Load(FreeImage_GetFileType(filename.cstr(), 0), filename.cstr()), filename.str(), width, height, realWidth, realHeight);
}

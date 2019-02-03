#include <Texture/Texture.h>
#include "TextureInternal.h"
#include <inttypes.h>

using namespace PGE;

int Texture::getWidth() const {
    return width;
}

int Texture::getHeight() const {
    return height;
}

bool Texture::isOpaque() const {
    return opaque;
}

BYTE* PGE::loadFIBuffer(String filename,int& width,int& height,int& realWidth,int& realHeight,bool& opaque) {
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.cstr(),0);

    FIBITMAP* temp = FreeImage_Load(format, filename.cstr());
    FIBITMAP* image = FreeImage_ConvertTo32Bits(temp);
    FreeImage_Unload(temp);

    int w = FreeImage_GetWidth(image);
    int h = FreeImage_GetHeight(image);

    width = w; height = h;

    int po2w = 1; int po2h = 1;
    while (po2w<w) { po2w*=2; }
    while (po2h<h) { po2h*=2; }

    if (w!=po2w || h!=po2h) {
        temp = image;
        image = FreeImage_Rescale(temp,po2w,po2h,FILTER_BILINEAR);
        FreeImage_Unload(temp);
    }

    realWidth = po2w; realHeight = po2h;

    BYTE* bits = FreeImage_GetBits(image);
    int bpp = FreeImage_GetBPP(image)/8;
    BYTE* newBits = new BYTE[realWidth*realHeight*bpp];

    SDL_Log("%d %d %d\n",po2w,po2h,bpp);

    opaque = true;
    for (int x=0;x<realWidth;x++) {
        for (int y=0;y<realHeight;y++) {
            if (opaque && bits[(x+(y*realWidth))*bpp+3]<255) {
                opaque = false;
            }

            int invY = realHeight-1-y;

            //flip vertically and convert from bgra to rgba
            newBits[(x+(y*realWidth))*bpp+0] = bits[(x+(invY*realWidth))*bpp+2];
            newBits[(x+(y*realWidth))*bpp+1] = bits[(x+(invY*realWidth))*bpp+1];
            newBits[(x+(y*realWidth))*bpp+2] = bits[(x+(invY*realWidth))*bpp+0];
            newBits[(x+(y*realWidth))*bpp+3] = bits[(x+(invY*realWidth))*bpp+3];
        }
    }

    FreeImage_Unload(image);

    return newBits;
}

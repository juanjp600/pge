#include <Texture/Texture.h>
#include "TextureInternal.h"
#include <inttypes.h>
#include "../Exception/Exception.h"

using namespace PGE;

int Texture::getWidth() const {
    return width;
}

int Texture::getHeight() const {
    return height;
}

BYTE* PGE::loadFIBuffer(const FilePath& filename,int& width,int& height,int& realWidth,int& realHeight) {
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.cstr(),0);

    FIBITMAP* temp = FreeImage_Load(format, filename.cstr());
    if (temp==nullptr) {
        throw Exception("loadFIBuffer","Failed to load "+filename.str());
    }
    FIBITMAP* image = FreeImage_ConvertTo32Bits(temp);
    if (image==nullptr) {
        throw Exception("loadFIBuffer","Failed to convert "+filename.str()+" to 32-bit RGBA");
    }
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
        if (image==nullptr) {
            throw Exception("loadFIBuffer","Failed to rescale "+filename.str()+" ("+String(w)+","+String(h)+" to "+String(po2w)+","+String(po2h)+")");
        }
        FreeImage_Unload(temp);
    }

    realWidth = po2w; realHeight = po2h;

    BYTE* bits = FreeImage_GetBits(image);
    if (bits==nullptr) {
        throw Exception("loadFIBuffer","Failed to retrieve data from "+filename.str());
    }
    int bpp = FreeImage_GetBPP(image)/8;
    BYTE* newBits = new BYTE[realWidth*realHeight*bpp];

    for (int x=0;x<realWidth;x++) {
        for (int y=0;y<realHeight;y++) {
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

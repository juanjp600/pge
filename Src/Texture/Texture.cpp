#include <Texture/Texture.h>
#include <FreeImage.h>
#include <inttypes.h>

using namespace PGE;

int Texture::getWidth() const {
    return width;
}

int Texture::getHeight() const {
    return height;
}

FIBITMAP* Texture::loadFIBuffer(String filename,int& width,int& height,int& realWidth,int& realHeight) {
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.cstr(),0);

    FIBITMAP* temp = FreeImage_Load(format, filename.cstr());
    FIBITMAP* image = FreeImage_ConvertTo32Bits(temp);
    FreeImage_Unload(temp);

    int w = FreeImage_GetWidth(image);
    int h = FreeImage_GetHeight(image);

    for (int x=0;x<w;x++) {
        for (int y=0;y<h;y++) {
            RGBQUAD rgba;
            FreeImage_GetPixelColor(image,x,y,&rgba);
            int red = rgba.rgbRed;
            int blue = rgba.rgbBlue;
            rgba.rgbRed = blue; rgba.rgbBlue = red;
            FreeImage_SetPixelColor(image,x,y,&rgba);
        }
    }

    width = w; height = h;

    int po2w = 1; int po2h = 1;
    while (po2w<w) { po2w*=2; }
    while (po2h<h) { po2h*=2; }

    temp = image;
    image = FreeImage_Rescale(temp,po2w,po2h,FILTER_BILINEAR);
    FreeImage_Unload(temp);

    realWidth = po2w; realHeight = po2h;

    return image;
}

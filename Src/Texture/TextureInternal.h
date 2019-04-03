#ifndef PGEINTERNAL_TEXTUREINTERNAL_H_INCLUDED
#define PGEINTERNAL_TEXTUREINTERNAL_H_INCLUDED

#include <Misc/String.h>
#include <FreeImage.h>

namespace PGE {

BYTE* loadFIBuffer(String filename, int& width, int& height, int& realWidth, int& realHeight, bool& opaque);

}

#endif

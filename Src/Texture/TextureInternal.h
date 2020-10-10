#ifndef PGEINTERNAL_TEXTUREINTERNAL_H_INCLUDED
#define PGEINTERNAL_TEXTUREINTERNAL_H_INCLUDED

#include <FreeImage.h>

namespace PGE {

BYTE* loadFIBuffer(const FilePath& filename, int& width, int& height, int& realWidth, int& realHeight);

}

#endif // PGEINTERNAL_TEXTUREINTERNAL_H_INCLUDED

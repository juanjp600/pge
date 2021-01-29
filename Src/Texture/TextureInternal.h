#ifndef PGEINTERNAL_TEXTUREINTERNAL_H_INCLUDED
#define PGEINTERNAL_TEXTUREINTERNAL_H_INCLUDED

#include <FreeImage.h>

namespace PGE {

BYTE* loadFIBufferFromMemory(const void* buffer, int size, int& width, int& height, int& realWidth, int& realHeight);
BYTE* loadFIBufferFromFile(const FilePath& filename, int& width, int& height, int& realWidth, int& realHeight);

}

#endif // PGEINTERNAL_TEXTUREINTERNAL_H_INCLUDED

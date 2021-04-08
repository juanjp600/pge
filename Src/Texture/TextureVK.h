#ifndef PGEINTERNAL_TEXTURE_VK_H_INCLUDED
#define PGEINTERNAL_TEXTURE_VK_H_INCLUDED

#include <Texture/Texture.h>

namespace PGE {

class TextureVK : public Texture {
    public:
        TextureVK(Graphics* gfx, int width, int height, bool renderTarget, const void* buffer, FORMAT fmt);
        TextureVK(Graphics* gfx, const FilePath& fn, ThreadManager* threadManager);
        TextureVK(Graphics* gfx, uint8_t* fiBuffer, int w, int h, int rw, int rh, const FilePath& fn);
        ~TextureVK();

        Texture* copy() const override;

        void* getNative() const override;
};

}

#endif // PGEINTERNAL_TEXTURE_VK_H_INCLUDED

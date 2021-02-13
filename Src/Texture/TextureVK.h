#ifndef PGEINTERNAL_TEXTURE_VK_H_INCLUDED
#define PGEINTERNAL_TEXTURE_VK_H_INCLUDED

#include <Texture/Texture.h>

namespace PGE {

class TextureVK : public Texture {
    public:
        TextureVK(Graphics* gfx, int width, int height, bool renderTarget, const void* buffer, FORMAT fmt);
        TextureVK(Graphics* gfx, const FilePath& fn, ThreadManager* threadManager);
        TextureVK(Graphics* gfx, uint8_t* fiBuffer, int w, int h, int rw, int rh, const FilePath& fn = FilePath::fromStr(""));
        ~TextureVK();

        virtual Texture* copy() const override;

        void* getNative() const override;

    private:
        void cleanup() override;
        void throwException(String func, String details) override;
};

}

#endif // PGEINTERNAL_TEXTURE_VK_H_INCLUDED

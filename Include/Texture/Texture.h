#ifndef PGE_TEXTURE_H_INCLUDED
#define PGE_TEXTURE_H_INCLUDED

#include <Misc/String.h>
#include <Graphics/Graphics.h>

namespace PGE {

class Texture {
    public:
        virtual bool isRenderTarget() const =0;

        int getWidth() const; int getHeight() const;
        bool isOpaque() const;

        static Texture* load(Graphics* gfx,String filename);
        static Texture* create(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer);
        ~Texture() {};
    protected:
        int width; int height;
        int realWidth; int realHeight;
        bool opaque;

        String filename; String name;

        Texture(){};
};

}

#endif

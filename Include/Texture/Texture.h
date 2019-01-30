#ifndef PGE_TEXTURE_H_INCLUDED
#define PGE_TEXTURE_H_INCLUDED

#include <Misc/String.h>
#include <Graphics/Graphics.h>

namespace PGE {

class Texture {
    public:
        enum class FORMAT {
            RGBA32,
            R32F
        };

        virtual bool isRenderTarget() const =0;

        int getWidth() const; int getHeight() const;
        bool isOpaque() const;

        static Texture* load(Graphics* gfx,String filename);
        static Texture* create(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer,FORMAT fmt);
        virtual ~Texture(){};
    protected:
        int width; int height;
        int realWidth; int realHeight;
        bool opaque;

        String filename; String name;
        FORMAT format;

        Texture(){};
};

}

#endif

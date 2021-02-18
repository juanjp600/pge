#ifndef PGE_TEXTURE_H_INCLUDED
#define PGE_TEXTURE_H_INCLUDED

#include <Misc/String.h>
#include <Misc/FilePath.h>
#include <Graphics/Graphics.h>
#include <Threading/ThreadManager.h>

namespace PGE {

class Texture {
    public:
        enum class FORMAT {
            RGBA32,
            R32F
        };

        bool isRenderTarget() const;

        int getWidth() const; int getHeight() const;

        virtual Texture* copy() const = 0;
        
        virtual void* getNative() const = 0;

        static Texture* load(Graphics* gfx, const void* buffer, int size);
        static Texture* load(Graphics* gfx, const FilePath& filename);
        static Texture* load(Graphics* gfx, const FilePath& filename, ThreadManager* threadManager);
        static Texture* create(Graphics* gfx, int w, int h, bool renderTarget, const void* buffer, FORMAT fmt);
        virtual ~Texture(){};

    protected:
        Graphics* graphics;
        bool isRT;

        int width; int height;
        int realWidth; int realHeight;

        FilePath filename; String name;
        FORMAT format;
};

}

#endif // PGE_TEXTURE_H_INCLUDED

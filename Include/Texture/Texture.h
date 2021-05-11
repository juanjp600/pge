#ifndef PGE_TEXTURE_H_INCLUDED
#define PGE_TEXTURE_H_INCLUDED

#include <String/String.h>
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

        static Texture* createRenderTarget(Graphics* gfx, int w, int h, FORMAT fmt);
        static Texture* createBlank(Graphics* gfx, int w, int h, FORMAT fmt);
        static Texture* load(Graphics* gfx, int w, int h, uint8_t* buffer, FORMAT fmt);
        virtual ~Texture() = default;

    protected:
        Texture(Graphics* gfx, int w, int h, bool rt, FORMAT fmt);

        Graphics* graphics;
        bool isRT;

        int width; int height;

        FORMAT format;
};

}

#endif // PGE_TEXTURE_H_INCLUDED

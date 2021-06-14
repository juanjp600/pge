#ifndef PGE_TEXTURE_H_INCLUDED
#define PGE_TEXTURE_H_INCLUDED

#include <PGE/String/String.h>
#include <PGE/File/FilePath.h>
#include <PGE/Graphics/Graphics.h>
#include <PGE/Threading/ThreadManager.h>

namespace PGE {

class Texture {
    public:
        enum class Format {
            RGBA32,
            R32F,
            R8,
        };
        static int getBytesPerPixel(Format fmt);

        bool isRenderTarget() const;

        int getWidth() const; int getHeight() const;
        Vector2i getDimensions() const;

        virtual Texture* copy() const = 0;
        
        virtual void* getNative() const = 0;

        static Texture* createRenderTarget(Graphics* gfx, int w, int h, Format fmt);
        static Texture* createBlank(Graphics* gfx, int w, int h, Format fmt, bool mipmaps);
        static Texture* load(Graphics* gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps = true);
        virtual ~Texture() = default;

    protected:
        Texture(Graphics* gfx, int w, int h, bool rt, Format fmt);

        Graphics* graphics;
        bool isRT;

        Vector2i dimensions;

        Format format;
};

}

#endif // PGE_TEXTURE_H_INCLUDED

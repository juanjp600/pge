#ifndef PGE_TEXTURE_H_INCLUDED
#define PGE_TEXTURE_H_INCLUDED

#include <String/String.h>
#include <Misc/FilePath.h>
#include <Graphics/Graphics.h>
#include <Threading/ThreadManager.h>

namespace PGE {

class Texture {
    public:
        enum class Format {
            RGBA32,
            R32F
        };

        bool isRenderTarget() const;

        int getWidth() const; int getHeight() const;
        Vector2i getDimensions() const;

        virtual Texture* copy() const = 0;
        
        virtual void* getNative() const = 0;

        static Texture* createRenderTarget(Graphics* gfx, int w, int h, Format fmt);
        static Texture* createBlank(Graphics* gfx, int w, int h, Format fmt);
        static Texture* load(Graphics* gfx, int w, int h, byte* buffer, Format fmt);
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

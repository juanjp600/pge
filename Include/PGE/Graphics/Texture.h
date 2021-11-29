#ifndef PGE_TEXTURE_H_INCLUDED
#define PGE_TEXTURE_H_INCLUDED

#include <PGE/Math/Vector.h>
#include <PGE/File/FilePath.h>
#include <PGE/Types/PolymorphicHeap.h>

namespace PGE {

class Graphics;

class Texture : private PolymorphicHeap {
    public:
        enum class Format {
            RGBA64,
            RGBA32,
            R32F,
            R8,
        };

        enum class CompressedFormat {
            BC1,
            BC2,
            BC3,
            BC4,
            BC5,
            BC6,
            BC7,
        };

        using AnyFormat = std::variant<Format, CompressedFormat>;

        static int getBytesPerPixel(Format fmt);

        bool isRenderTarget() const;

        int getWidth() const; int getHeight() const;
        Vector2i getDimensions() const;
        
        virtual void* getNative() const = 0;

        static Texture* createRenderTarget(Graphics& gfx, int w, int h, Format fmt);
        static Texture* createBlank(Graphics& gfx, int w, int h, Format fmt, bool mipmaps);
        static Texture* load(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps = true);
        struct Mipmap {
            int width; int height; const byte* buffer; size_t size;
            Mipmap(int width, int height, const byte* buffer, size_t size)
                : width(width), height(height), buffer(buffer), size(size) { }
            Mipmap() = default;
        };
        static Texture* loadCompressed(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt);
        virtual ~Texture() = default;

    protected:
        Texture(int w, int h, bool rt, const AnyFormat& fmt);

        const bool isRT;
        const Vector2i dimensions;
        const AnyFormat format;
};

}

#endif // PGE_TEXTURE_H_INCLUDED

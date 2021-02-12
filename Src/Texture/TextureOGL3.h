#ifndef PGEINTERNAL_TEXTUREOGL3_H_INCLUDED
#define PGEINTERNAL_TEXTUREOGL3_H_INCLUDED

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

#include <Texture/Texture.h>
#include <Threading/ThreadManager.h>

namespace PGE {

class TextureOGL3 : public Texture {
    public:
        TextureOGL3(Graphics* gfx, int w, int h, bool renderTarget = false, const void* buffer = nullptr, FORMAT fmt = FORMAT::RGBA32);
        TextureOGL3(Graphics* gfx, const FilePath& fn, ThreadManager* threadManager);
        TextureOGL3(Graphics* gfx, uint8_t* fiBuffer, int w, int h, int rw, int rh, const FilePath& fn = FilePath::fromStr(""));
        ~TextureOGL3();

        virtual Texture* copy() const;

        GLuint getGlTexture() const;
        //GLuint getGlFramebuffer() const;
        GLuint getGlDepthbuffer() const;
        virtual void* getNative() const;

    private:
        void cleanup() override;
        void throwException(String func, String details) override;

        GLuint glTexture;
        //GLuint glFramebuffer;
        GLuint glDepthbuffer;
};

}

#endif // PGEINTERNAL_TEXTUREOGL3_H_INCLUDED

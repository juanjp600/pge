#ifndef PGEINTERNAL_TEXTUREOGL3_H_INCLUDED
#define PGEINTERNAL_TEXTUREOGL3_H_INCLUDED

#include <Texture/Texture.h>
#include <Threading/ThreadManager.h>

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

namespace PGE {

class TextureOGL3 : public Texture {
    public:
        TextureOGL3(Graphics* gfx,int w,int h,bool renderTarget=false,const void* buffer=nullptr,FORMAT fmt=FORMAT::RGBA32);
        TextureOGL3(Graphics* gfx,const FilePath& fn);
        TextureOGL3(Graphics* gfx,const FilePath& fn,ThreadManager* threadManager);
        virtual ~TextureOGL3();

        virtual bool isRenderTarget() const override;

        virtual Texture* copy() const;

        GLuint getGlTexture() const;
        //GLuint getGlFramebuffer() const;
        GLuint getGlDepthbuffer() const;
        virtual void* getNative() const;
    private:
        TextureOGL3(){};

        void cleanup() override;
        void throwException(String func, String details) override;

        bool isRT;

        Graphics* graphics;

        GLuint glTexture;
        //GLuint glFramebuffer;
        GLuint glDepthbuffer;
};

}

#endif

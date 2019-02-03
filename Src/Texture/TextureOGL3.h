#ifndef PGEINTERNAL_TEXTUREOGL3_H_INCLUDED
#define PGEINTERNAL_TEXTUREOGL3_H_INCLUDED

#include <GL/glew.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/GL.h>
#endif

namespace PGE {

class TextureOGL3 : public Texture {
    public:
        TextureOGL3(Graphics* gfx,int w,int h,bool renderTarget=false,const void* buffer=nullptr);
        TextureOGL3(Graphics* gfx,const String& fn);
        virtual ~TextureOGL3();

        virtual bool isRenderTarget() const;

        GLuint getGlTexture() const;
    private:
        TextureOGL3(){};

        bool isRT;

        Graphics* graphics;

        GLuint glTexture;
        GLuint glFramebuffer;
};

}

#endif

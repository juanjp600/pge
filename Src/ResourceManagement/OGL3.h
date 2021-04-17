#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_OGL3
#define PGEINTERNAL_RESOURCEMANAGEMENT_OGL3

#include <vector>

#include <ResourceManagement/ResourceManager.h>

#include <GL/glew.h>
#include <SDL.h>

namespace PGE {

class GLContext : public Resource<SDL_GLContext> {
    public:
        GLContext(SDL_Window* window) {
            resource = SDL_GL_CreateContext(window);
            // And make it later in the day.
            SDL_GL_MakeCurrent(window, resource);
        }

        ~GLContext() {
            SDL_GL_DeleteContext(resource);
        }

        __RES_MNGMT__REF_FACT_METH(GLContext)
};

class GLFramebuffer : public Resource<GLuint> {
    public:
        GLFramebuffer() {
            glGenFramebuffers(1, &resource);
            GLenum glError = glGetError();
            __ASSERT(glError == GL_NO_ERROR, "Failed to generate frame buffer (GLERROR: " + String::format(glError, "%u") + ")");
        }
        
        ~GLFramebuffer() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &resource);
        }

        __RES_MNGMT__REF_FACT_METH(GLFramebuffer)
};

class GLBuffer : public Resource<GLuint> {
    public:
        GLBuffer() {
            glGenBuffers(1, &resource);
        }

        ~GLBuffer() {
            glDeleteBuffers(1, &resource);
        }

        __RES_MNGMT__REF_FACT_METH(GLBuffer)
};

class GLVertexArray : public Resource<GLuint> {
    public:
        GLVertexArray() {
            glGenVertexArrays(1, &resource);
        }

        ~GLVertexArray() {
            glDeleteVertexArrays(1, &resource);
        }

        __RES_MNGMT__REF_FACT_METH(GLVertexArray)
};

class GLTexture : public Resource<GLuint> {
    public:
        GLTexture() {
            glGenTextures(1, &resource);
        }

        ~GLTexture() {
            glDeleteTextures(1, &resource);
        }

        __RES_MNGMT__REF_FACT_METH(GLTexture)
};

class GLDepthBuffer : public Resource<GLuint> {
    public:
        GLDepthBuffer(int width, int height) {
            glGenRenderbuffers(1, &resource);
            glBindRenderbuffer(GL_RENDERBUFFER, resource);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        ~GLDepthBuffer() {
            glDeleteRenderbuffers(1, &resource);
        }

        __RES_MNGMT__REF_FACT_METH(GLDepthBuffer)
};

class GLShader : public Resource<GLuint> {
    public:
        GLShader(GLenum stage, const String& source) {
            resource = glCreateShader(stage);
            const char* src = source.cstr();
            glShaderSource(resource, 1, &src, nullptr);
            glCompileShader(resource);

            char err[512];
            glGetShaderInfoLog(resource, 512, NULL, err);

            int result;
            glGetShaderiv(resource, GL_COMPILE_STATUS, &result);
            __ASSERT(result == GL_TRUE, "Failed to create shader (stage: " + String::format(stage, "%u") + "; error:" + err + ")");
        }

        ~GLShader() {
            glDeleteShader(resource);
        }

        __RES_MNGMT__REF_FACT_METH(GLShader)
};

class GLProgram : public Resource<GLuint> {
    public:
        GLProgram(const std::vector<GLuint>& shaders) {
            resource = glCreateProgram();
            for (GLuint s : shaders) {
                glAttachShader(resource, s);
            }
            glLinkProgram(resource);
            GLint result;
            glGetProgramiv(resource, GL_LINK_STATUS, &result);
            __ASSERT(result == GL_TRUE, "Failed to link shader (GLERROR:" + String::format(glGetError(), "%u") + ")");
        }

        ~GLProgram() {
            glDeleteProgram(resource);
        }

        __RES_MNGMT__REF_FACT_METH(GLProgram)
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_OGL3

#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_OGL3
#define PGEINTERNAL_RESOURCEMANAGEMENT_OGL3

#include <vector>

#include <glad/gl.h>
#include <SDL.h>

#include <PGE/ResourceManagement/ResourceManager.h>

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
};

class GLFramebuffer : public Resource<GLuint> {
    public:
        GLFramebuffer() {
            glGenFramebuffers(1, &resource);
            GLenum glError = glGetError();
            PGE_ASSERT(glError == GL_NO_ERROR, "Failed to generate frame buffer (GLERROR: " + String::from(glError) + ")");
        }
        
        ~GLFramebuffer() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &resource);
        }
};

class GLBuffer : public Resource<GLuint> {
    public:
        GLBuffer() {
            glGenBuffers(1, &resource);
        }

        ~GLBuffer() {
            glDeleteBuffers(1, &resource);
        }
};

class GLVertexArray : public Resource<GLuint> {
    public:
        GLVertexArray() {
            glGenVertexArrays(1, &resource);
        }

        ~GLVertexArray() {
            glDeleteVertexArrays(1, &resource);
        }
};

class GLTexture : public Resource<GLuint> {
    public:
        GLTexture() {
            glGenTextures(1, &resource);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, resource);
        }

        ~GLTexture() {
            glDeleteTextures(1, &resource);
        }
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
};

class GLShader : public Resource<GLuint> {
    public:
        GLShader(GLenum stage, const String& source) {
            resource = glCreateShader(stage);
            const char* src = source.cstr();
            glShaderSource(resource, 1, &src, nullptr);
            glCompileShader(resource);

            GLint result;
            glGetShaderiv(resource, GL_COMPILE_STATUS, &result);
            if (result != GL_TRUE) {
                glGetShaderiv(resource, GL_INFO_LOG_LENGTH, &result);
                std::unique_ptr<GLchar[]> err = std::make_unique<GLchar[]>(result);
                glGetShaderInfoLog(resource, result, NULL, err.get());
                throw PGE_CREATE_EX("Failed to create shader (stage: " + String::from(stage) + "; error:\n" + err.get() + ")");
            }
        }

        ~GLShader() {
            glDeleteShader(resource);
        }
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
            PGE_ASSERT(result == GL_TRUE, "Failed to link shader (GLERROR:" + String::from(glGetError()) + ")");
        }

        ~GLProgram() {
            glDeleteProgram(resource);
        }
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_OGL3

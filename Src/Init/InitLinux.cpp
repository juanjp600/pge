#include <Init/Init.h>

#include <SDL.h>

static void InitEnv() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

static void QuitEnv() {
    SDL_Quit();
}

int main(int argc, char** argv) {
    InitEnv();
    int retVal = PGE::Main();
    QuitEnv();
    return retVal;
}

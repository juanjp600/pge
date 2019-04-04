#include <Init/Init.h>

#include <SDL.h>

#include "InitInternal.h"

void PGE::InitEnv() {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

void PGE::QuitEnv() {
    SDL_Quit();
}

int main(int argc, char** argv) {
    return PGE::InitMain();
}

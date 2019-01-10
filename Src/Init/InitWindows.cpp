#include <Init/Init.h>

#include <SDL.h>

namespace PGE {

void InitEnv() {
    SDL_Init(SDL_INIT_VIDEO);
}

void QuitEnv() {
    SDL_Quit();
}

}

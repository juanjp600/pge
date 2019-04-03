#include "../Exception/Exception.h"
#include <Misc/String.h>
#include <Init/Init.h>

#include <SDL.h>
#include <fstream>

#include "InitInternal.h"

void PGE::InitEnv() {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
}

void PGE::QuitEnv() {
    SDL_Quit();
}

int WinMain() {
    return PGE::InitMain();
}

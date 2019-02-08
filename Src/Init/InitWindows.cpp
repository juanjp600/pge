#include <Init/Init.h>

#include <SDL.h>

static void InitEnv() {
    SDL_Init(SDL_INIT_VIDEO);
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

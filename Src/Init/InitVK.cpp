#include "InitInternal.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#ifdef VULKAN
    void PGE::InitEnv() {
        SDL_SetMainReady();
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

        SDL_GameControllerEventState(SDL_ENABLE);
        SDL_JoystickEventState(SDL_ENABLE);
    }

    void PGE::QuitEnv() {
        SDL_Quit();
    }

    #ifdef DEBUG
    int main() {
    #else
    int WinMain() {
    #endif
        return PGE::InitMain();
    }
#endif

#include "../Exception/Exception.h"
#include <Misc/String.h>
#include <Init/Init.h>

#include <SDL.h>
#include <fstream>

static void InitEnv() {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
}

static void QuitEnv() {
    SDL_Quit();
}

int WinMain() {
    try {
        InitEnv();
        int retVal = PGE::Main();
        QuitEnv();
        return retVal;
    } catch (PGE::Exception e) {
        std::ofstream exceptionLog;
        exceptionLog.open("exception.txt",std::ofstream::out | std::ofstream::trunc);
        exceptionLog.write(e.getSource().cstr(),e.getSource().size());
        exceptionLog.write("\n",1);
        exceptionLog.write(e.getDetails().cstr(),e.getDetails().size());
        exceptionLog.write("\n",1);
        SDL_ShowSimpleMessageBox(0,"Fatal Error",
                (PGE::String("An exception has been thrown, please send \"exception.txt\" to a developer.\nException source: ")+e.getSource()).cstr(),nullptr);
    }
}

#include <SDL.h>
#include <Init/Init.h>
#include <Exception/Exception.h>
#include <fstream>

#include "InitInternal.h"

int PGE::InitMain() {
#ifndef DEBUG
    try {
#endif
        InitEnv();
        int retVal = PGEMain::Main();
        QuitEnv();
        return retVal;
#ifndef DEBUG
    } catch (PGE::Exception& e) {
        std::ofstream exceptionLog;
        exceptionLog.open("exception.txt",std::ofstream::out | std::ofstream::trunc);
        PGE::String exceptionType = "PGE::Exception";
        exceptionLog.write(exceptionType.cstr(),exceptionType.size());
        exceptionLog.write("\n",1);
        exceptionLog.write(e.getSource().cstr(),e.getSource().size());
        exceptionLog.write("\n",1);
        exceptionLog.write(e.getDetails().cstr(),e.getDetails().size());
        exceptionLog.write("\n",1);
        exceptionLog.close();
        SDL_ShowSimpleMessageBox(0,"Fatal Error",
                (PGE::String("An exception has been thrown, please send \"exception.txt\" to a developer.\nException source: ")+e.getSource()).cstr(),nullptr);
        return -1;
    } catch (std::exception& e) {
        std::ofstream exceptionLog;
        exceptionLog.open("exception.txt",std::ofstream::out | std::ofstream::trunc);
        PGE::String exceptionType = "std::exception";
        exceptionLog.write(exceptionType.cstr(),exceptionType.size());
        exceptionLog.write("\n",1);
        PGE::String details = e.what();
        exceptionLog.write(details.cstr(),details.size());
        exceptionLog.write("\n",1);
        exceptionLog.close();
        SDL_ShowSimpleMessageBox(0,"Fatal Error",
            "An exception has been thrown, please send \"exception.txt\" to a developer.\nException was std::exception",nullptr);
        return -1;
    }
#endif
}

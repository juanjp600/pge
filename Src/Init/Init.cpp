#include <SDL.h>
#include <Init/Init.h>
#include <Exception/Exception.h>
#include <fstream>
#include <wtypes.h>

#ifdef DEBUG
int main(int argc, char** argv) {
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    try {
#endif
        SDL_SetMainReady();
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

        SDL_GameControllerEventState(SDL_ENABLE);
        SDL_JoystickEventState(SDL_ENABLE);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        int retVal = PGEMain::Main();

        SDL_Quit();
        return retVal;
#ifndef DEBUG
    } catch (PGE::Exception& e) {
        std::ofstream exceptionLog;
        exceptionLog.open("exception.txt", std::ofstream::out | std::ofstream::trunc);
        PGE::String exceptionType = "PGE::Exception";
        exceptionLog.write(exceptionType.cstr(), exceptionType.byteLength());
        exceptionLog.write("\n", 1);
        exceptionLog.write(e.getSource().cstr(), e.getSource().byteLength());
        exceptionLog.write("\n", 1);
        exceptionLog.write(e.getDetails().cstr(), e.getDetails().byteLength());
        exceptionLog.write("\n", 1);
        exceptionLog.close();
        SDL_ShowSimpleMessageBox(0, "Fatal Error",
            (PGE::String("An exception has been thrown, please send \"exception.txt\" to a developer.\nException source: ") + e.getSource()).cstr(), nullptr);
        return -1;
    } catch (std::exception& e) {
        std::ofstream exceptionLog;
        exceptionLog.open("exception.txt", std::ofstream::out | std::ofstream::trunc);
        PGE::String exceptionType = "std::exception";
        exceptionLog.write(exceptionType.cstr(), exceptionType.byteLength());
        exceptionLog.write("\n", 1);
        PGE::String details = e.what();
        exceptionLog.write(details.cstr(), details.byteLength());
        exceptionLog.write("\n", 1);
        exceptionLog.close();
        SDL_ShowSimpleMessageBox(0, "Fatal Error",
            "An exception has been thrown, please send \"exception.txt\" to a developer.\nException was std::exception", nullptr);
        return -1;
    }
#endif
}

#include <SDL.h>

#ifndef DEBUG
#include <wtypes.h>
#endif

#include <PGE/Init/Init.h>
#include <PGE/Graphics/Graphics.h>
#include <PGE/Exception/Exception.h>
#include <PGE/File/FileWriter.h>
#include <PGE/Info/Info.h>

using namespace PGE;

static void showError(const String& exceptionType, const String& what) {
    FileWriter writer = FileWriter(FilePath::fromStr("exception.txt"));
    writer.writeLine(Info::REPO_LINK);
    writer.writeLine(Info::BRANCH + " - " + Info::COMMIT);
    writer.writeLine(exceptionType);
    writer.writeLine(what);
    String activeGraphics = "Active graphics: \n";
    for (Graphics* gfx : Graphics::getActiveInstances()) {
        activeGraphics += gfx->getInfo();
    }
    writer.writeLine(activeGraphics);
    SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Fatal Error",
        "An exception has been thrown, please send \"exception.txt\" to a developer.", NULL);
}

void Init::init() {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_JoystickEventState(SDL_ENABLE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

void Init::quit() {
    SDL_Quit();
}

#ifdef DEBUG
int main(int argc, char** argv) {
#else
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ INT nCmdShow) {
    try {
#endif
        Init::init();

#ifndef DEBUG
        int argc;
        wchar** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
#endif

        std::vector<String> args(argc);
        for (int i = 0; i < argc; i++) {
            args[i] = argv[i];
        }

        int retVal = Init::main(args);

        Init::quit();
        
        return retVal;
#ifndef DEBUG
    } catch (const Exception& e) {
        showError("PGE::Exception", e.what());
    } catch (const std::exception& e) {
        showError("std::exception", e.what());
    } catch (...) {
        showError("Unknown", "???");
    }
    return -1;
#endif
}

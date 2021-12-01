#include <SDL.h>

#ifndef DEBUG
#include <wtypes.h>
#endif

#include <PGE/Init/Init.h>
#include <PGE/Graphics/Graphics.h>
#include <PGE/Exception/Exception.h>
#include <PGE/File/TextWriter.h>
#include <PGE/Info/Info.h>

#include "../Graphics/GraphicsInternal.h"

using namespace PGE;

static void assertSDL(int retCode, const std::source_location& location = std::source_location::current()) {
    assert(retCode >= 0, SDL_GetError(), location);
}

#ifndef DEBUG
static void showError(const String& exceptionType, const String& what) {
    TextWriter writer = TextWriter(FilePath::fromStr("exception.txt"));
    writer.writeLine(Info::REPO_LINK);
    writer.writeLine(Info::BRANCH + " - " + Info::COMMIT);
    writer.writeLine(exceptionType);
    writer.writeLine(what);
    SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Fatal Error",
        "An exception has been thrown, please send \"exception.txt\" to a developer.", NULL);
}
#endif

// Throws PGE::Exception on error.
void Init::init() {
    SDL_SetMainReady();
    assertSDL(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC));

    SDL_GameControllerEventState(SDL_ENABLE);
    assertSDL(SDL_JoystickEventState(SDL_ENABLE));

    assertSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
    assertSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3));
    assertSDL(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3));
    assertSDL(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8));
}

void Init::quit() {
    SDL_Quit();
}

#ifdef DEBUG
int main(int argc, char** argv) {
#else
#pragma warning(push)
#pragma warning(disable: 4100) // Unusued parameters.
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ INT nCmdShow) {
#pragma warning(pop)
    try {
#endif
        Init::init();

#ifndef DEBUG
        int convArgc;
        char16** convArgv = CommandLineToArgvW(GetCommandLineW(), &convArgc);
#else
        int convArgc = argc;
        char** convArgv = argv;
#endif

        std::vector<String> args(convArgc);
        for (int i = 0; i < convArgc; i++) {
            args[i] = convArgv[i];
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

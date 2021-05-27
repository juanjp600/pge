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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        int retVal = PGEMain::Main();

        SDL_Quit();
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

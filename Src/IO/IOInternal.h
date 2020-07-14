#ifndef PGEINTERNAL_IOINTERNAL_H_INCLUDED
#define PGEINTERNAL_IOINTERNAL_H_INCLUDED

#include <IO/IO.h>

#include <vector>
#include <SDL.h>

namespace PGE {

class IOInternal : public IO {
    private:
        Window* window;

        SysEvents::Subscriber* keyboardSubscriber;
        SysEvents::Subscriber* mouseSubscriber;
        SysEvents::Subscriber* controllerSubscriber;
        SysEvents::Subscriber* textSubscriber;

        std::set<UserInput*> inputs;
        Vector2f mousePos;
        Vector2i mouseWheelPos;

        std::vector<class ControllerInternal*> openControllers;

        // The last received string from SDL_TEXTINPUT events.
        String textInput;

    public:
        IOInternal(Window* win);
        ~IOInternal();

        void update() override;

        void trackInput(UserInput* input) override;
        void untrackInput(UserInput* input) override;

        int getControllerCount() const override;
        Controller* getController(int index) const override;
        bool isControllerValid(Controller* controller) const override;

        Vector2f getMousePosition() const override;
        void setMousePosition(Vector2f position) override;
        void setMouseVisibility(bool visible) override;
        Vector2i getMouseWheelDelta() override;

        void startTextInputCapture() const override;
        void stopTextInputCapture() const override;
        String getTextInput() const override;

        void setClipboardText(String str) const override;
        String getClipboardText() const override;
};

class ControllerInternal : public Controller {
    private:
        bool removed;
        const IOInternal* io;
        SDL_GameController* sdlController;
        String name;
    public:
        ~ControllerInternal() override;
        ControllerInternal(const IOInternal* inIo, SDL_GameController* inSdlController);
        String getName() const override;
        void rumble(float lowFreqIntensity, float highFreqIntensity, int durationMs) override;
        SDL_GameController* getSdlController() const;
        void setName(const String& inName);
};

}

#endif


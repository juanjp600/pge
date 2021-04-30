#ifndef PGEINTERNAL_IOINTERNAL_H_INCLUDED
#define PGEINTERNAL_IOINTERNAL_H_INCLUDED

#include <unordered_set>
#include <vector>

#include <SDL.h>

#include <IO/IO.h>

namespace PGE {

class Graphics;

class IOInternal : public IO {
    private:
        Graphics* graphics;

        SysEvents::Subscriber* keyboardSubscriber;
        SysEvents::Subscriber* mouseSubscriber;
        SysEvents::Subscriber* controllerSubscriber;
        SysEvents::Subscriber* textSubscriber;

        std::unordered_set<UserInput*> inputs;
        Vector2f mousePos;
        Vector2i mouseWheelPos;

        std::vector<class ControllerInternal*> openControllers;

        // The last received string from SDL_TEXTINPUT events.
        String textInput;

    public:
        IOInternal(Graphics* gfx);
        ~IOInternal();

        void update() override;

        void trackInput(UserInput* input) override;
        void untrackInput(UserInput* input) override;

        int getControllerCount() const override;
        Controller* getController(int index) const override;
        bool isControllerValid(Controller* controller) const override;

        const Vector2f& getMousePosition() const override;
        void setMousePosition(const Vector2f& position) override;
        void setMouseVisibility(bool visible) override;
        Vector2i getMouseWheelDelta() override;

        void startTextInputCapture() const override;
        void stopTextInputCapture() const override;
        String getTextInput() const override;

        void setClipboardText(const String& str) const override;
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
        const String& getName() const override;
        void rumble(float lowFreqIntensity, float highFreqIntensity, int durationMs) override;
        SDL_GameController* getSdlController() const;
        void setName(const String& inName);
};

}

#endif // PGEINTERNAL_IOINTERNAL_H_INCLUDED

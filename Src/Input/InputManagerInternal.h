#ifndef PGEINTERNAL_INPUTMANAGERINTERNAL_H_INCLUDED
#define PGEINTERNAL_INPUTMANAGERINTERNAL_H_INCLUDED

#include <unordered_set>
#include <vector>

#include <SDL.h>

#include <PGE/Input/InputManager.h>

namespace PGE {

class InputManagerInternal : public InputManager {
    private:
        const Graphics& graphics;

        SysEvents::Subscriber* keyboardSubscriber;
        SysEvents::Subscriber* mouseSubscriber;
        SysEvents::Subscriber* controllerSubscriber;
        SysEvents::Subscriber* textSubscriber;

        std::unordered_set<Input*> inputs;
        Vector2f mousePos;
        Vector2i mouseWheelPos;

        std::vector<class ControllerInternal*> openControllers;

        // The last received string from SDL_TEXTINPUT events.
        String textInput;

    public:
        InputManagerInternal(const Graphics& gfx);
        ~InputManagerInternal();

        void update() override;

        void trackInput(Input* input) override;
        void untrackInput(Input* input) override;

        int getControllerCount() const override;
        Controller* getController(int index) const override;
        bool isControllerValid(Controller* controller) const override;

        const Vector2f& getMousePosition() const override;
        void setMousePosition(const Vector2f& position) override;
        void setMouseVisibility(bool visible) override;
        Vector2i getMouseWheelDelta() override;

        void startTextInputCapture() const override;
        void stopTextInputCapture() const override;
        const String& getTextInput() const override;

        void setClipboardText(const String& str) const override;
        String getClipboardText() const override;
};

class ControllerInternal : public Controller {
    private:
        bool removed;
        const InputManagerInternal& io;
        SDL_GameController& sdlController;
        String name;

    public:
        ~ControllerInternal() override;
        ControllerInternal(const InputManagerInternal& inIo, SDL_GameController& inSdlController);
        const String& getName() const override;
        void rumble(float lowFreqIntensity, float highFreqIntensity, int durationMs) override;
        SDL_GameController& getSdlController() const;
        void setName(const String& inName);
};

}

#endif // PGEINTERNAL_INPUTMANAGERINTERNAL_H_INCLUDED

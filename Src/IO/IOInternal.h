#ifndef PGEINTERNAL_IOINTERNAL_H_INCLUDED
#define PGEINTERNAL_IOINTERNAL_H_INCLUDED

#include <IO/IO.h>

#include <map>
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

        std::map<Controller*, SDL_GameController*> openControllers;

        // The last received string from SDL_TEXTINPUT events.
        String textInput;

    public:
        IOInternal(Window* win);
        ~IOInternal();

        void update() override;

        void trackInput(UserInput* input) override;
        void untrackInput(UserInput* input) override;

        int getControllerCount() const override;
        Controller* openController(int index) override;
        void closeController(Controller* controller) override;

        Vector2f getMousePosition() const override;
        void setMousePosition(Vector2f position) override;
        void setMouseVisibility(bool visible) override;

        void startTextInputCapture() const override;
        void stopTextInputCapture() const override;
        String getTextInput() const override;

        void setClipboardText(String str) const override;
        String getClipboardText() const override;
};

}

#endif


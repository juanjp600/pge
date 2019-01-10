#ifndef PGE_INPUT_H_INCLUDED
#define PGE_INPUT_H_INCLUDED

#include <SDL.h>

namespace PGE {

class UserInput {
    public:
        enum class DEVICE {
            KEYBOARD,
            MOUSE,
            GAMEPAD
        };

        virtual DEVICE getDevice() const = 0;

        bool isDown() const;
        void setDown(bool down);
    protected:
        UserInput();

        bool inputDown;
};

class KeyboardInput : public UserInput {
    public:
        virtual DEVICE getDevice() const;

        SDL_Scancode getButton() const;

        KeyboardInput(SDL_Scancode inKeyCode);
    private:
        SDL_Scancode keyCode;
};

class MouseInput : public UserInput {
    public:
        enum class BUTTON {
            LEFT,
            RIGHT,
            MIDDLE,
            SIDE1,
            SIDE2
        };

        virtual DEVICE getDevice() const;

        BUTTON getButton() const;

        MouseInput(BUTTON inMouseButton);
    private:
        BUTTON mouseButton;
};

class GamepadInput : public UserInput {
    public:
        virtual DEVICE getDevice() const;

        SDL_GameControllerButton getButton() const;

        GamepadInput(SDL_GameControllerButton inGamepadButton);
    private:
        SDL_GameControllerButton gamepadButton;
};

}

#endif

